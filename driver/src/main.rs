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

use rppal::gpio::Gpio;

use crate::{
    gantry::{Gantry, GantryValue},
    motor::{Motor, StepMotor},
    port::Magnet,
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

        const MAX_Z_POWER: f64 = 45.0;
        const MOVE_SCALE: f64 = 25.0;
        const MAX_CLAMP: f64 = 150.0;

        let move_gantry_x = value.x.signum()
            * (1.0 / value.x.powi(2) * MAX_Z_POWER.powi(2) * MOVE_SCALE)
                .clamp(-MAX_CLAMP, MAX_CLAMP);
        let move_gantry_y = -(value.y.signum()
            * (1.0 / value.y.powi(2) * MAX_Z_POWER.powi(2) * MOVE_SCALE)
                .clamp(-MAX_CLAMP, MAX_CLAMP));

        if value.z.abs() > MAX_Z_POWER {
            continue;
        }

        let value = GantryValue::new()
            .move_x(move_gantry_x as i32)
            .move_y(move_gantry_y as i32);

        if !args.no_track {
            gantry.move_gantry(value);
        } else {
            println!("Move value: {value}");
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
}
