mod eventqueue;
mod port;

use clap::Parser;
use serde::Deserialize;
use std::{
    thread::{self, sleep},
    time::Duration,
};
use tungstenite::{connect, Message};

use rppal::{gpio::Gpio, pwm::{Channel, Pwm}};

use crate::port::Serial;

/// SG90 on hardware PWM0 (GPIO 12, ALT0).
/// 50 Hz period = 20 ms; pulse width 500–2400 µs maps to 0–180°.
const SERVO_PERIOD: Duration = Duration::from_millis(20);
const SERVO_MIN_US: u64 = 500;  // 0°
const SERVO_MAX_US: u64 = 2400; // 180°

/// Grid width must match the backend board (default 10).
const GRID_WIDTH: f64 = 10.0;

/// Map 0–180° to pulse width in µs.
fn angle_to_pulse_us(angle: f64) -> u64 {
    let angle = angle.clamp(0.0, 180.0);
    let us = SERVO_MIN_US as f64 + (angle / 180.0) * (SERVO_MAX_US - SERVO_MIN_US) as f64;
    us.round() as u64
}

/// Map grid X (0..GRID_WIDTH-1) to pulse width in µs.
fn x_to_pulse_us(x: f64) -> u64 {
    let angle = (x / (GRID_WIDTH - 1.0)) * 180.0;
    angle_to_pulse_us(angle)
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

    // Hardware PWM0 is always on GPIO 12 (ALT0) — requires dtoverlay=pwm,pin=12,func=4
    let servo = Pwm::new(Channel::Pwm0).expect("Failed to open hardware PWM0 (GPIO 12). Is dtoverlay=pwm,pin=12,func=4 set in /boot/firmware/config.txt?");
    servo.set_period(SERVO_PERIOD).unwrap();
    let center_us = (SERVO_MIN_US + SERVO_MAX_US) / 2;
    servo.set_pulse_width(Duration::from_micros(center_us)).unwrap();
    servo.enable().unwrap();
    println!("SG90 on hardware PWM0 (GPIO 12), centred at {center_us}µs");

    loop {
        match socket.read() {
            Ok(Message::Text(text)) => {
                let Ok(value) = serde_json::from_str::<serde_json::Value>(&text) else {
                    continue;
                };

                match value.get("type").and_then(|t| t.as_str()) {
                    Some("driver_servo_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverServoCommand>(value) {
                            let pulse_us = angle_to_pulse_us(cmd.angle);
                            println!("Servo → {:.1}° ({}µs)", cmd.angle, pulse_us);
                            servo.set_pulse_width(Duration::from_micros(pulse_us)).unwrap();
                        }
                    }
                    Some("driver_move_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverMoveCommand>(value) {
                            let pulse_us = x_to_pulse_us(cmd.x.min(GRID_WIDTH as u64 - 1) as f64);
                            println!(
                                "Moving {} → x={} ({}µs)  [cmd: {}]",
                                cmd.figure_id, cmd.x, pulse_us, cmd.command_id
                            );
                            servo.set_pulse_width(Duration::from_micros(pulse_us)).unwrap();
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

    servo.disable().ok();
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
