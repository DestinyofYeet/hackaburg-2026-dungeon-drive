use std::{
    collections::VecDeque,
    fmt::Write,
    io,
    sync::{Arc, Mutex},
    thread::{self, JoinHandle},
    time::Duration,
};

use serialport::SerialPort;

#[derive(Debug)]
pub struct SerialValue {
    pub x: f64,
    pub y: f64,
    pub z: f64,
}

impl SerialValue {
    pub fn parse(string: &str) -> Option<Self> {
        let parts = string.split(",").collect::<Vec<&str>>();

        if parts.len() < 3 {
            return None;
        }

        let x = parts[0].parse::<f64>();
        let y = parts[1].parse::<f64>();
        let z = parts[2].parse::<f64>();

        match (x, y, z) {
            (Ok(x), Ok(y), Ok(z)) => {
                if x.is_nan() || y.is_nan() || z.is_nan() {
                    return None;
                }

                Some(Self { x, y, z })
            }
            _ => None,
        }
    }
}

pub struct Magnet {
    buffer: Arc<Mutex<VecDeque<SerialValue>>>,
    read_handle: JoinHandle<()>,
}

impl Magnet {
    pub fn new(port: String, rate: u32) -> Self {
        let port = serialport::new(port, rate)
            .timeout(Duration::from_millis(10))
            .open()
            .unwrap();

        let buffer = Arc::new(Mutex::new(VecDeque::new()));

        let handle = Self::read_thread(port, buffer.clone());

        Self {
            buffer: buffer.clone(),
            read_handle: handle,
        }
    }

    fn read_thread(
        mut port: Box<dyn SerialPort>,
        buffer: Arc<Mutex<VecDeque<SerialValue>>>,
    ) -> JoinHandle<()> {
        let handle = thread::spawn(move || {
            let mut byte_buffer: Vec<u8> = vec![0; 100];
            let mut local_buffer = String::new();

            loop {
                match port.read(&mut byte_buffer) {
                    Ok(read) => {
                        local_buffer += String::from_utf8_lossy(&byte_buffer[..read]).as_ref();
                    }
                    Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
                    Err(e) => panic!("Failed to read from port: {e}"),
                }

                let mut buffer = buffer.lock().expect("to get lock");

                let mut last_index = 0;
                // println!("Buffer: {local_buffer}");
                for (index, char) in local_buffer.char_indices() {
                    if char == '\n' {
                        let lines = local_buffer[last_index..index].lines();

                        for line in lines {
                            if line.is_empty() {
                                continue;
                            }

                            let line = line.trim();

                            // println!("window {line}");

                            if let Some(value) = SerialValue::parse(line) {
                                // println!("last_index: {last_index}");
                                last_index = index;
                                buffer.push_back(value);
                            }
                        }
                    }
                }

                local_buffer = local_buffer[last_index..local_buffer.len()].to_string();

                drop(buffer);
            }
        });

        println!("Created thread");

        handle
    }

    pub fn read_value(&mut self) -> Option<SerialValue> {
        let mut buffer = self.buffer.lock().expect("to get lock");

        buffer.pop_front()
    }
}
