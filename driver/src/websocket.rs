use std::{
    collections::VecDeque,
    net::TcpStream,
    sync::{Arc, Mutex},
    thread::{self, JoinHandle},
};

use serde_json::Value;
use websocket::{
    ClientBuilder, OwnedMessage,
    sync::{Reader, Writer},
};

pub struct WSConn {
    writer: Writer<TcpStream>,
    queue: Arc<Mutex<VecDeque<WebsocketCommand>>>,

    read_handle: JoinHandle<()>,
}

#[derive(Debug, Clone)]
pub enum WebsocketCommand {
    DriverMoveCommand { x: i64, y: i64 },
    RespondPong(Vec<u8>),
}

impl WSConn {
    pub fn new(url: String) -> Self {
        let socket = ClientBuilder::new(&url)
            .unwrap()
            .connect_insecure()
            .unwrap();

        let (reader, writer) = socket.split().unwrap();

        let queue: Arc<Mutex<VecDeque<WebsocketCommand>>> = Default::default();
        let handle = Self::read_data(reader, queue.clone());
        Self {
            writer,
            queue,
            read_handle: handle,
        }
    }

    fn read_data(
        mut reader: Reader<TcpStream>,
        queue: Arc<Mutex<VecDeque<WebsocketCommand>>>,
    ) -> JoinHandle<()> {
        thread::spawn(move || {
            for message in reader.incoming_messages() {
                let message = message.unwrap();
                println!("{message:?}");

                let mut queue = queue.lock().expect("to get lock");
                match message {
                    websocket::OwnedMessage::Text(text) => {
                        let value: Value = serde_json::from_str(&text).unwrap();

                        match value.get("type") {
                            Some(Value::String(string)) => {
                                if string != "driver_move_command" {
                                    continue;
                                }
                            }
                            _ => continue,
                        }

                        let x = value.get("x").unwrap().as_i64().unwrap();
                        let y = value.get("y").unwrap().as_i64().unwrap();

                        println!("New message");
                        queue.push_back(WebsocketCommand::DriverMoveCommand { x, y });
                    }
                    websocket::OwnedMessage::Binary(_) => continue,
                    websocket::OwnedMessage::Close(_) => {
                        reader.shutdown_all().unwrap();
                        return;
                    }
                    websocket::OwnedMessage::Ping(e) => {
                        queue.push_back(WebsocketCommand::RespondPong(e))
                    }
                    websocket::OwnedMessage::Pong(_) => continue,
                }
            }

            println!("Exited websocket thread");
        })
    }

    pub fn get_value(&self) -> Option<WebsocketCommand> {
        self.queue.lock().expect("to get lock").pop_front()
    }

    pub fn peek_value(&self) -> Option<WebsocketCommand> {
        self.queue.lock().expect("to get lock").front().cloned()
    }

    pub fn send_pong(&mut self, data: Vec<u8>) {
        self.writer.send_message(&OwnedMessage::Pong(data)).unwrap()
    }
}
