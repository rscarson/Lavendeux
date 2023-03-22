#![cfg_attr(
	all(not(debug_assertions), target_os = "windows"),
	windows_subsystem = "windows"
)]

use single_instance::SingleInstance;

extern crate lavendeux_parser;
use lavendeux_parser::ParserState;

mod core;
mod ui;
mod utils;

use crate::{
	core::{ settings::{ Settings }, language },
	ui:: { app::{ App } },
	utils::{ logs }
};

mod parser;

const INSTANCE_LOCK: &str = "lavendeux-instance-lock";

fn main() {
	// Initialize base state
	let config = Settings::create().unwrap_or_default();
	let mut state = core::State {
		logger: logs::Manager::new(Some(config.inner_settings.logname.clone())),
		parser: ParserState::new(),
		settings: config.clone(),
		history: Vec::new(),
		language: language::initialize()
	};

	// Check for single instance
	let instance = SingleInstance::new(INSTANCE_LOCK).unwrap();
	if !instance.is_single() {
		state.logger.error("Another instance of Lavendeux is already running.");
		return;
	}

	// Configure tauri app
	let app = App::new(&state);
	app.run();
}
