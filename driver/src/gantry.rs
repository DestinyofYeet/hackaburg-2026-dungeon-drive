use std::time::Duration;

use serialport::SerialPort;

pub enum GantryValue {
    X(i32),
    Y(i32),
    Z(i32),
}

#[allow(clippy::to_string_trait_impl)]
impl ToString for GantryValue {
    fn to_string(&self) -> String {
        match self {
            GantryValue::X(value) => format!("x: {value}"),
            GantryValue::Y(value) => format!("y: {value}"),
            GantryValue::Z(value) => format!("z: {value}"),
        }
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

    pub fn write(&mut self, value: GantryValue) {
        self.port.write_all(value.to_string().as_bytes()).unwrap()
    }
}
