use std::{thread, time::Duration};

use rppal::gpio::{Gpio, OutputPin};

pub struct Motor {
    step_pin: OutputPin,
    direction_pin: OutputPin,
}

pub enum StepMotor {
    Forward { steps: u64 },
    Backward { steps: u64 },
}

impl Motor {
    pub fn new(step_pin: u8, direction_pin: u8) -> Self {
        let step = Gpio::new()
            .unwrap()
            .get(step_pin)
            .unwrap()
            .into_output_low();
        let direction = Gpio::new()
            .unwrap()
            .get(direction_pin)
            .unwrap()
            .into_output_low();

        Self {
            step_pin: step,
            direction_pin: direction,
        }
    }

    pub fn step_motor(&mut self, step: StepMotor) {
        let mut is_foward = false;
        let do_steps;
        match step {
            StepMotor::Forward { steps } => {
                do_steps = steps;
                is_foward = true;
            }
            StepMotor::Backward { steps } => do_steps = steps,
        }

        if !is_foward {
            self.direction_pin.set_high();
        }

        for _ in 0..do_steps {
            self.step_pin.set_high();
            thread::sleep(Duration::from_millis(100));
            self.step_pin.set_low();
            thread::sleep(Duration::from_millis(100));
        }
    }
}
