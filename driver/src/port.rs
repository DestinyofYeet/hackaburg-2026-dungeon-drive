use serialport::SerialPort;

#[derive(Debug)]
pub struct SerialValue {
    x: f64,
    y: f64,
    z: f64,
}

pub struct Serial {
    port: Box<dyn SerialPort>,
}

impl Serial {
    pub fn new(port: String, rate: u32) -> Self {
        let port = serialport::new(port, rate).open().unwrap();

        Self { port }
    }

    pub fn read_value(&mut self) -> Option<SerialValue> {
        let mut value = String::new();
        self.port.read_to_string(&mut value).unwrap();

        println!("Received magnetic value: {value}");

        let parts = value.split(",").map(|e| e.trim()).collect::<Vec<&str>>();

        let x = parts[0].parse::<f64>();
        let y = parts[1].parse::<f64>();
        let z = parts[2].parse::<f64>();

        match (x, y, z) {
            (Ok(x), Ok(y), Ok(z)) => Some(SerialValue { x, y, z }),
            _ => None,
        }
    }
}
