use std::{
    fmt::Debug,
    io::Read,
    net::TcpStream,
    sync::mpsc::{self, Receiver, Sender},
    thread::{self, JoinHandle},
};

use websocket::{
    ClientBuilder,
    sync::{Client, Reader, Writer},
};

pub enum Event {
    MoveMotor { x: u8, y: u8 },
}

pub struct EventManager {
    receiver: Receiver<Event>,
    sender: Sender<Event>,
}

impl EventManager {
    pub fn new() -> Self {
        let (sender, receiver) = mpsc::channel();

        Self { receiver, sender }
    }

    pub fn get_sender(&self) -> Sender<Event> {
        self.sender.clone()
    }
}

pub struct Websocket {
    sender: Writer<TcpStream>,
    rx_join_handle: JoinHandle<()>,
}

impl Websocket {
    pub fn new(address: String) -> Self {
        println!("Connecting to ws...");
        let client = ClientBuilder::new(&address)
            .unwrap()
            .connect_insecure()
            .unwrap();

        let (receiver, sender) = client.split().unwrap();

        let rx_join_handle = Self::on_message(receiver);

        Self {
            sender,
            rx_join_handle,
        }
    }

    fn on_message(mut receiver: Reader<TcpStream>) -> JoinHandle<()> {
        thread::spawn(move || {
            for message in receiver.incoming_messages() {
                let message = match message {
                    Ok(e) => e,
                    Err(e) => {
                        eprintln!("Failed to receive message: {e}");
                        return;
                    }
                };

                println!("Received: {message:?}");
            }
        })
    }
}
