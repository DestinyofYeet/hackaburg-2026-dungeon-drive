mod eventqueue;
mod gantry;
mod motor;
mod port;

use clap::Parser;
use serde::Deserialize;
use std::{
    thread::{self, sleep},
    time::Duration,
};
use tungstenite::{connect, Message};

use rppal::gpio::Gpio;

use crate::{
    gantry::{Gantry, GantryValue},
    motor::{Motor, StepMotor},
    port::Serial,
};

#[derive(Parser, Debug)]
pub struct Args {
    /// WebSocket URL of the backend.
    #[arg(short, long)]
    url: Option<String>,

    #[arg(short = None, long = "magnet")]
    magnet_serial: Option<String>,

    #[arg(short, long = "gantry")]
    gantry_serial: Option<String>,
}

fn main() {
    let args = Args::parse();

    if let Some(serial) = args.magnet_serial {
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

    if let Some(serial) = args.gantry_serial {
        let mut gantry = Gantry::new(serial);

        gantry.write(GantryValue::X(10));
    }

    // ── Servo + WebSocket driver mode ─────────────────────────────────────────
    if args.servo {
        let url = args
            .url
            .unwrap_or_else(|| "ws://127.0.0.1:8000/ws".to_string());
        run_servo_driver(&url);
    }
}
