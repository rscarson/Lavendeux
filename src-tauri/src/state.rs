use crate::history::History;
use crate::settings::Settings;
use crate::logs::LogManager;
use lavendeux_parser::ParserState;
use std::sync::Mutex;

/// Current system state
#[derive(Clone)]
pub struct State {
    pub parser: ParserState,
    pub settings: Settings,
    pub logger: LogManager,
    pub history: Vec<History>
}

// Shared mutable state
pub struct SharedState(pub Mutex<State>);