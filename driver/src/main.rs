mod eventqueue;

use std::{thread, time::Duration};

use rppal::gpio::Gpio;

fn main() {
    let mut motor = Gpio::new().unwrap().get(17).unwrap().into_output_low();

    loop {
        println!("Stepping");
        for _ in 0..1 {
            motor.toggle();
        }

        println!("Done");

        thread::sleep(Duration::from_secs(1));
    }
}
