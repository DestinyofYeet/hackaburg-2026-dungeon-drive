mod eventqueue;
mod port;

use clap::Parser;
use serde::Deserialize;
use std::{
    thread::{self, sleep},
    time::Duration,
};
use tungstenite::{connect, Message};

use rppal::gpio::Gpio;

use crate::port::Serial;

/// SG90 servo: 50 Hz, duty cycle 2.5% (0°) to 12.5% (180°).
const SERVO_FREQ_HZ: f64 = 50.0;
const SERVO_MIN_DUTY: f64 = 2.5;  // 0°
const SERVO_MAX_DUTY: f64 = 12.5; // 180°

const GRID_WIDTH: f64 = 10.0;

fn angle_to_duty(angle: f64) -> f64 {
    SERVO_MIN_DUTY + (angle.clamp(0.0, 180.0) / 180.0) * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)
}

fn x_to_duty(x: f64) -> f64 {
    angle_to_duty((x / (GRID_WIDTH - 1.0)) * 180.0)
}

#[derive(Deserialize, Debug)]
struct DriverServoCommand {
    angle: f64,
}

#[derive(Deserialize, Debug)]
struct DriverMoveCommand {
    figure_id: String,
    x: u64,
    y: u64,
    command_id: String,
}

fn run_servo_driver(ws_url: &str) {
    println!("Connecting to backend: {ws_url}");
    let (mut socket, _) = connect(ws_url).expect("Failed to connect to backend WebSocket");
    println!("Connected. Waiting for servo/move commands…");

    let mut pin = Gpio::new()
        .expect("Failed to init GPIO")
        .get(17)
        .expect("Failed to get GPIO 17")
        .into_output();

    let center_duty = angle_to_duty(90.0);
    pin.set_pwm_frequency(SERVO_FREQ_HZ, center_duty / 100.0)
        .expect("Failed to start software PWM");
    println!("SG90 on GPIO 17 (software PWM), centred at {center_duty:.1}%");

    loop {
        match socket.read() {
            Ok(Message::Text(text)) => {
                let Ok(value) = serde_json::from_str::<serde_json::Value>(&text) else {
                    continue;
                };

                match value.get("type").and_then(|t| t.as_str()) {
                    Some("driver_servo_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverServoCommand>(value) {
                            let duty = angle_to_duty(cmd.angle);
                            println!("Servo → {:.1}° ({:.2}% duty)", cmd.angle, duty);
                            pin.set_pwm_frequency(SERVO_FREQ_HZ, duty / 100.0)
                                .expect("Failed to set PWM");
                        }
                    }
                    Some("driver_move_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverMoveCommand>(value) {
                            let duty = x_to_duty(cmd.x.min(GRID_WIDTH as u64 - 1) as f64);
                            println!(
                                "Moving {} → x={} ({:.2}% duty)  [cmd: {}]",
                                cmd.figure_id, cmd.x, duty, cmd.command_id
                            );
                            pin.set_pwm_frequency(SERVO_FREQ_HZ, duty / 100.0)
                                .expect("Failed to set PWM");
                        }
                    }
                    _ => {}
                }
            }
            Ok(Message::Close(_)) => {
                println!("WebSocket closed by server, exiting.");
                break;
            }
            Err(e) => {
                eprintln!("WebSocket error: {e}");
                break;
            }
            _ => {}
        }
    }

    pin.clear_pwm().ok();
}

#[derive(Parser, Debug)]
pub struct Args {
    /// WebSocket URL of the backend.
    #[arg(short, long)]
    url: Option<String>,

    #[arg(short = 'p', long)]
    pin: Option<u8>,

    #[arg(short = None, long)]
    serial: Option<String>,

    /// Enable servo mode (SG90 via hardware PWM0 on GPIO 12).
    #[arg(long, action = clap::ArgAction::SetTrue)]
    servo: bool,
}

fn main() {
    let args = Args::parse();

    // ── Serial reader mode ────────────────────────────────────────────────────
    if let Some(serial) = args.serial {
        let mut port = Serial::new(serial, 9_600);
        loop {
            let value = port.read_value();
            println!("value: {:?}", value);
            sleep(Duration::from_millis(50));
        }
    } else {
        println!(
            "Available ports: {:?}",
            serialport::available_ports().unwrap()
        );
    }

    // ── GPIO step-pin mode ────────────────────────────────────────────────────
    if let Some(pin) = args.pin {
        let mut motor = Gpio::new().unwrap().get(pin).unwrap().into_output_low();
        loop {
            println!("Stepping");
            motor.toggle();
            println!("Done");
            thread::sleep(Duration::from_secs(1));
        }
    }

    // ── Servo + WebSocket driver mode ─────────────────────────────────────────
    if args.servo {
        let url = args
            .url
            .unwrap_or_else(|| "ws://127.0.0.1:8000/ws".to_string());
        run_servo_driver(&url);
    }
}
