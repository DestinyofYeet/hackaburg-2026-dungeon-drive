use std::io::Write;
use std::{fmt::Display, time::Duration};

use serialport::SerialPort;

pub struct GantryValue {
    x: i32,
    y: i32,
    z: i32,
}

impl GantryValue {
    pub fn new() -> Self {
        Self { x: 0, y: 0, z: 0 }
    }

    pub fn move_x(mut self, value: i32) -> Self {
        self.x = value;
        self
    }

    pub fn move_y(mut self, value: i32) -> Self {
        self.y = value;
        self
    }
}

impl Display for GantryValue {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_fmt(format_args!("{},{},{}", self.x, self.y, self.z))
    }
}

pub struct Gantry {
    port: Box<dyn SerialPort>,
}

impl Gantry {
    pub fn new(serial_port: String) -> Self {
        let port = serialport::new(serial_port, 115_200)
            .timeout(Duration::from_millis(10))
            .open()
            .unwrap();

        Self { port }
    }

    pub fn move_gantry(&mut self, value: GantryValue) {
        println!("Sending command: {}", value);
        self.write_raw(&format!("{}\n", value));
    }

    pub fn write_raw(&mut self, value: &str) {
        self.port.write_all(value.as_bytes()).unwrap()
    }
}
