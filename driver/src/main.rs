mod gantry;
mod motor;
mod port;
mod websocket;

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
    websocket::{WSConn, WebsocketCommand},
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
    url: String,

    #[arg(short = None, long = "magnet")]
    magnet_serial: String,

    #[arg(short, long = "gantry")]
    gantry_serial: String,

    #[arg(short = None, long)]
    no_track: bool,

    #[arg(short, long)]
    manual: bool,

    #[arg(short = None, long)]
    no_prompt: bool,
}

fn main() {
    let args = Args::parse();

    println!(
        "Available ports: {:?}",
        serialport::available_ports().unwrap()
    );

    let mut magnet_serial = Magnet::new(args.magnet_serial, 9_600);

    let mut gantry = Gantry::new(args.gantry_serial);

    if args.manual {
        loop {
            gantry.write_raw(&get_input("Input"));
        }
    }

    let mut ws = WSConn::new(args.url);

    let mut last_value: Option<GantryValue> = None;

    loop {
        if let Some(WebsocketCommand::RespondPong(data)) = ws.peek_value() {
            _ = ws.get_value();
            ws.send_pong(data);
        }

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

        const MAX_Z_POWER: f64 = 33.0;
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

            if args.no_prompt {
                continue;
            }

            let (x, y) = match ws.get_value() {
                Some(WebsocketCommand::DriverMoveCommand { x, y }) => (x, y),
                _ => continue,
            };

            magnet_serial.write_enable(false);

            let pickup_offset = GantryValue::new().move_x(5100).move_y(-500).move_z(180);
            gantry.move_gantry(pickup_offset);

            sleep(Duration::from_secs(2));

            let basic_move = GantryValue::new()
                .move_y(((y + x) * -500 * 2) as i32)
                .move_x((x * 300) as i32)
                .move_z(180);
            gantry.move_gantry(basic_move);

            let drop_off = GantryValue::new().move_z(1);
            gantry.move_gantry(drop_off);

            sleep(Duration::from_secs(2));

            let revert_basic_move = -basic_move;
            gantry.move_gantry(revert_basic_move);

            let revert_pickup = -pickup_offset;
            gantry.move_gantry(revert_pickup);

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
