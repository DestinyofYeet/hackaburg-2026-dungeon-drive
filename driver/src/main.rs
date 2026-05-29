mod eventqueue;
mod port;

use clap::Parser;
use serde::Deserialize;
use std::{
    net::TcpStream,
    thread::{self, sleep},
    time::Duration,
};
use tungstenite::{connect, stream::MaybeTlsStream, Message, WebSocket};

use rppal::{
    gpio::Gpio,
    pwm::Pwm,
};

use crate::port::Serial;

/// Carson CS3 servo PWM constants (50 Hz, standard hobby servo).
const SERVO_PERIOD_MS: u64 = 20;
const SERVO_MIN_US: u64 = 1000; // 0° (fully left)
const SERVO_MAX_US: u64 = 2000; // 180° (fully right)

/// Grid dimensions must match the backend board width (default 10).
const GRID_WIDTH: u64 = 10;

/// Map a grid X coordinate (0..GRID_WIDTH-1) to a servo pulse width in µs.
fn x_to_pulse_us(x: u64) -> u64 {
    let range = SERVO_MAX_US - SERVO_MIN_US;
    SERVO_MIN_US + (x * range) / (GRID_WIDTH - 1)
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
    println!("Connected. Waiting for driver_move_command messages…");

    let servo = Pwm::new(rppal::pwm::Channel::Pwm0).expect("Failed to open PWM0 (GPIO 18)");
    servo.set_period(Duration::from_millis(SERVO_PERIOD_MS)).unwrap();
    servo.set_pulse_width(Duration::from_micros(1500)).unwrap(); // centre on startup
    servo.enable().unwrap();
    println!("Servo initialised on PWM0 (GPIO 18), centred at 1500 µs");

    loop {
        match socket.read() {
            Ok(Message::Text(text)) => {
                let Ok(value) = serde_json::from_str::<serde_json::Value>(&text) else {
                    continue;
                };

                if value.get("type").and_then(|t| t.as_str()) != Some("driver_move_command") {
                    continue;
                }

                let cmd: DriverMoveCommand = match serde_json::from_value(value) {
                    Ok(c) => c,
                    Err(e) => {
                        eprintln!("Failed to parse driver_move_command: {e}");
                        continue;
                    }
                };

                let pulse_us = x_to_pulse_us(cmd.x.min(GRID_WIDTH - 1));
                println!(
                    "Moving {} → x={} (pulse {}µs)  [cmd: {}]",
                    cmd.figure_id, cmd.x, pulse_us, cmd.command_id
                );
                servo.set_pulse_width(Duration::from_micros(pulse_us)).unwrap();
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
}

#[derive(Parser, Debug)]
pub struct Args {
    /// WebSocket URL of the backend. When provided together with --servo,
    /// the driver listens for move commands and controls the servo.
    #[arg(short, long)]
    url: Option<String>,

    #[arg(short = 'p', long)]
    pin: Option<u8>,

    #[arg(short = None, long)]
    serial: Option<String>,

    /// Enable servo mode (Carson CS3 on PWM0 / GPIO 18).
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
