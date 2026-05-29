mod eventqueue;
mod port;

use clap::Parser;

use std::{
    thread::{self, sleep},
    time::Duration,
};

use rppal::gpio::Gpio;

use crate::port::Serial;

#[derive(Parser, Debug)]
pub struct Args {
    #[arg(short, long)]
    url: Option<String>,

    #[arg(short = 'p', long)]
    pin: Option<u8>,

    #[arg(short, long)]
    serial: Option<String>,
}

fn main() {
    let args = Args::parse();

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

    if let Some(pin) = args.pin {
        let mut motor = Gpio::new().unwrap().get(pin).unwrap().into_output_low();

        loop {
            println!("Stepping");
            for _ in 0..1 {
                motor.toggle();
            }

            println!("Done");

            thread::sleep(Duration::from_secs(1));
        }
    }
}
