use lavendeux_parser::ParserState;
use std::sync::Mutex;
use embedded_lang::LanguageSet;

use crate::core::history::History;
use crate::core::settings::Settings;
use crate::utils::logs;

/// Current system state
#[derive(Clone)]
pub struct State {
    pub parser: ParserState,
    pub settings: Settings,
    pub logger: logs::Manager,
    pub history: Vec<History>,
    pub language: LanguageSet,
}

// Shared mutable state
pub struct SharedState(pub Mutex<State>);