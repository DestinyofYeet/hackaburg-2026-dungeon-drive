mod eventqueue;
mod gantry;
mod motor;
mod port;

use clap::Parser;
use std::{
    thread::sleep,
    time::Duration,
};

use crate::{
    gantry::{Gantry, GantryValue},
    port::Serial,
};

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
}

fn main() {
    let args = Args::parse();

    // ── Magnet sensor serial reader mode ──────────────────────────────────────
    if let Some(serial) = args.magnet_serial {
        let mut port = Serial::new(serial, 9_600);

        loop {
            let value = port.read_value();
            println!("value: {:?}", value);
            sleep(Duration::from_millis(50));
        }
    }

    // ── Gantry serial mode ────────────────────────────────────────────────────
    if let Some(serial) = args.gantry_serial {
        let mut gantry = Gantry::new(serial);
        gantry.write(GantryValue::X(10));
        return;
    }

    // ── No mode selected ──────────────────────────────────────────────────────
    println!(
        "Available ports: {:?}",
        serialport::available_ports().unwrap()
    );
}
