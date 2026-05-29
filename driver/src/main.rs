mod eventqueue;

use clap::Parser;

use std::{thread, time::Duration};

use rppal::gpio::Gpio;

use crate::eventqueue::Websocket;

#[derive(Parser, Debug)]
pub struct Args {
    #[arg(short = 'w', long)]
    websocket: Option<String>,

    #[arg(short = 'p', long)]
    pin: Option<u8>,
}

fn main() {
    let args = Args::parse();

    if let Some(socket_addr) = args.websocket {
        let ws = Websocket::new(socket_addr);
        println!("Connected to ws");
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
