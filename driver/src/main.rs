mod eventqueue;
mod motor;
mod port;

use clap::Parser;

use std::{
    thread::{self, sleep},
    time::Duration,
};

use rppal::{
    gpio::Gpio,
    pwm::{Polarity, Pwm},
};

use crate::{motor::Motor, port::Serial};

#[derive(Parser, Debug)]
pub struct Args {
    #[arg(short, long)]
    url: Option<String>,

    #[arg(short = None, long)]
    serial: Option<String>,

    #[arg(short = None, long)]
    servo: Option<u8>,
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

    {
        let mut motor_x = Motor::new(5, 6);
        let mut motor_y = Motor::new(23, 24);
    }

    // if let Some(pin) = args.pin {
    //     let mut motor = Gpio::new().unwrap().get(pin).unwrap().into_output_low();

    //     loop {
    //         println!("Stepping");
    //         for _ in 0..1 {
    //             motor.toggle();
    //         }

    //         println!("Done");

    //         thread::sleep(Duration::from_secs(1));
    //     }
    // }

    if args.servo.is_some() {
        let motor = Pwm::new(rppal::pwm::Channel::Pwm0).unwrap();
        motor.set_pulse_width(Duration::from_micros(1500)).unwrap();
        motor.set_period(Duration::from_millis(20)).unwrap();

        motor.enable().unwrap();
        let mut last = Polarity::Normal;
        loop {
            println!("Stepping");

            match last {
                Polarity::Normal => {
                    motor.set_polarity(Polarity::Inverse).unwrap();
                    last = Polarity::Inverse;
                }
                Polarity::Inverse => {
                    motor.set_polarity(Polarity::Normal).unwrap();
                    last = Polarity::Normal;
                }
            }
            sleep(Duration::from_secs(1));
        }
    }
}
