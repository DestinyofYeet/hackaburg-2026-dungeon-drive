mod eventqueue;
mod gantry;
mod motor;
mod port;

use clap::Parser;
use serde::Deserialize;
use std::{
    io::Write,
    thread::{self, sleep},
    time::Duration,
};
use tungstenite::{connect, Message};

use rppal::gpio::Gpio;
use serialport::SerialPort;

use crate::{
    gantry::{Gantry, GantryValue},
    motor::{Motor, StepMotor},
    port::Magnet,
};

/// SG90 servo: 50 Hz, duty cycle 2.5% (0°) to 12.5% (180°).
const SERVO_FREQ_HZ: f64 = 50.0;
const SERVO_MIN_DUTY: f64 = 2.5; // 0°
const SERVO_MAX_DUTY: f64 = 12.5; // 180°

const GRID_WIDTH: f64 = 10.0;

fn angle_to_duty(angle: f64) -> f64 {
    SERVO_MIN_DUTY + (angle.clamp(0.0, 180.0) / 180.0) * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)
}

fn x_to_angle(x: f64) -> f64 {
    (x / (GRID_WIDTH - 1.0)) * 180.0
}

fn send_angle(target: &mut ServoTarget, angle: f64) {
    let angle = angle.clamp(0.0, 180.0) as u8;
    match target {
        ServoTarget::Serial(port) => {
            let cmd = format!("servo: {angle}\n");
            port.write_all(cmd.as_bytes()).expect("Failed to write to serial");
        }
        ServoTarget::Gpio(pin) => {
            let duty = angle_to_duty(angle as f64);
            pin.set_pwm_frequency(SERVO_FREQ_HZ, duty / 100.0)
                .expect("Failed to set PWM");
        }
    }
}

enum ServoTarget {
    Serial(Box<dyn SerialPort>),
    Gpio(rppal::gpio::OutputPin),
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

fn run_servo_driver(ws_url: &str, servo_serial: Option<String>) {
    println!("Connecting to backend: {ws_url}");
    let (mut socket, _) = connect(ws_url).expect("Failed to connect to backend WebSocket");
    println!("Connected. Waiting for servo/move commands…");

    let mut target = if let Some(port_name) = servo_serial {
        println!("Servo via serial: {port_name}");
        let port = serialport::new(port_name, 115_200)
            .timeout(std::time::Duration::from_millis(10))
            .open()
            .expect("Failed to open servo serial port");
        ServoTarget::Serial(port)
    } else {
        println!("Servo via software PWM on GPIO 17");
        let pin = Gpio::new()
            .expect("Failed to init GPIO")
            .get(17)
            .expect("Failed to get GPIO 17")
            .into_output();
        ServoTarget::Gpio(pin)
    };

    send_angle(&mut target, 90.0); // centre on startup
    println!("SG90 centred at 90°");

    loop {
        match socket.read() {
            Ok(Message::Text(text)) => {
                let Ok(value) = serde_json::from_str::<serde_json::Value>(&text) else {
                    continue;
                };

                match value.get("type").and_then(|t| t.as_str()) {
                    Some("driver_servo_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverServoCommand>(value) {
                            println!("Servo → {:.1}°", cmd.angle);
                            send_angle(&mut target, cmd.angle);
                        }
                    }
                    Some("driver_move_command") => {
                        if let Ok(cmd) = serde_json::from_value::<DriverMoveCommand>(value) {
                            let angle = x_to_angle(cmd.x.min(GRID_WIDTH as u64 - 1) as f64);
                            println!(
                                "Moving {} → x={} ({:.1}°) [cmd: {}]",
                                cmd.figure_id, cmd.x, angle, cmd.command_id
                            );
                            send_angle(&mut target, angle);
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

    if let ServoTarget::Gpio(pin) = &mut target {
        pin.clear_pwm().ok();
    }
}

#[derive(Parser, Debug)]
pub struct Args {
    /// WebSocket URL of the backend.
    #[arg(short, long)]
    url: Option<String>,

    /// Serial port for the magnet sensor.
    #[arg(short = None, long = "magnet")]
    magnet_serial: Option<String>,

    /// Serial port for the gantry controller.
    #[arg(short, long = "gantry")]
    gantry_serial: Option<String>,

    #[arg(short, long)]
    no_track: bool,
}

fn main() {
    let args = Args::parse();

    println!(
        "Available ports: {:?}",
        serialport::available_ports().unwrap()
    );

    if args.magnet_serial.is_none() {
        eprintln!("Magnet serial is needed!");
        return;
    }

    if args.gantry_serial.is_none() {
        eprintln!("Gantry serial is needed!");
        return;
    }

    let mut magnet_serial = Magnet::new(args.magnet_serial.unwrap(), 9_600);

    let mut gantry = Gantry::new(args.gantry_serial.unwrap());

    loop {
        let value = match magnet_serial.read_value() {
            None => continue,
            Some(value) => value,
        };

        println!("{value:?}");

        let move_gantry_x = value.x % 20.0 * 20.0;
        let move_gantry_y = -(value.y % 20.0 * 20.0);

        if value.z.abs() > 45.0 {
            continue;
        }

        let value = GantryValue::new()
            .move_x(move_gantry_x as i32)
            .move_y(move_gantry_y as i32);

        if !args.no_track {
            gantry.move_gantry(value);
        }

        // let mut line = String::new();
        // print!("Write command: ");
        // std::io::stdout().flush().unwrap();
        // std::io::stdin().read_line(&mut line).unwrap();

        // gantry.write_raw(line);

        // let value = GantryValue::new().move_x(2000).move_y(2000);
        // gantry.move_gantry(value);

        // sleep(Duration::from_secs(2));

        // let value = GantryValue::new().move_x(-2000).move_y(-2000);
        // gantry.move_gantry(value);

        // sleep(Duration::from_secs(2));
    }

    // ── Servo + WebSocket driver mode ─────────────────────────────────────────
    if args.servo {
        let url = args
            .url
            .unwrap_or_else(|| "ws://127.0.0.1:8000/ws".to_string());

        run_servo_driver(&url, args.servo_serial);
        return;
    }

    // ── No mode selected ──────────────────────────────────────────────────────
    println!(
        "Available ports: {:?}",
        serialport::available_ports().unwrap()
    );
}