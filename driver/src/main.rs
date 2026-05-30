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

pub fn get_input(prompt: &str) -> String {
    print!("{prompt}: ");
    std::io::stdout().flush().unwrap();
    let mut line = String::new();
    std::io::stdin().read_line(&mut line).unwrap();
    line.trim().to_string()
}

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

    #[arg(short, long)]
    manual: bool,
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

    if args.manual {
        loop {
            gantry.write_raw(&get_input("Input"));
        }
    }

    // pickup offset: 4300, -500

    let mut last_value: Option<GantryValue> = None;

    loop {
        let value = match magnet_serial.read_value() {
            None => {
                if let Some(last_value) = last_value {
                    println!("Last try");
                    gantry.move_gantry(last_value);
                }

                last_value = None;
                continue;
            }
            Some(value) => value,
        };

        println!("{value:?}");

        const MAX_Z_POWER: f64 = 70.0;
        const MOVE_SCALE: f64 = 25.0;
        const MAX_CLAMP: f64 = 130.0;

        let move_gantry_x = value.y.signum()
            * (1.0 / value.y.powi(2) * MAX_Z_POWER.powi(2) * MOVE_SCALE)
                .clamp(-MAX_CLAMP, MAX_CLAMP);
        let move_gantry_y = value.x.signum()
            * (1.0 / value.x.powi(2) * MAX_Z_POWER.powi(2) * MOVE_SCALE)
                .clamp(-MAX_CLAMP, MAX_CLAMP);

        if value.z.abs() > MAX_Z_POWER {
            if args.no_track {
                continue;
            }

            magnet_serial.write_enable(false);

            if get_input("ready to track? y/N").to_lowercase() != "y" {
                magnet_serial.clear();
                magnet_serial.write_enable(true);
                continue;
            };

            let pickup_offset = GantryValue::new().move_x(5100).move_y(-500).move_z(180);
            gantry.move_gantry(pickup_offset);

            sleep(Duration::from_secs(2));

            let basic_move = GantryValue::new().move_y(2000).move_z(180);
            gantry.move_gantry(basic_move);

            let drop_off = GantryValue::new().move_z(1);
            gantry.move_gantry(drop_off);

            sleep(Duration::from_secs(1));

            let move_back = GantryValue::new().move_x(-2000).move_y(3000);
            gantry.move_gantry(move_back);

            magnet_serial.write_enable(true);
            magnet_serial.clear();

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

        last_value = Some(value);
    }
}
