use std::{thread, time::Duration};

use rust_gpiozero::DigitalOutputDevice;

fn main() {
    let mut motor = DigitalOutputDevice::new(17);

    loop {
        println!("Stepping");
        for _ in 0..200 {
            motor.toggle();
        }

        println!("Done");

        thread::sleep(Duration::from_secs(1));
    }
}
