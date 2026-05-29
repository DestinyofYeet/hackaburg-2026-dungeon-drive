use std::sync::mpsc::{self, Receiver, Sender};

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
