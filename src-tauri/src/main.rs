#![cfg_attr(
	all(not(debug_assertions), target_os = "windows"),
	windows_subsystem = "windows"
)]

use std::{thread, time};
use tauri::api::cli::get_matches;
use tauri::{RunEvent, WindowEvent, Manager};
use std::sync::Mutex;
use single_instance::SingleInstance;

extern crate lavendeux_parser;
use lavendeux_parser::ParserState;

mod logs;
pub use logs::*;

mod windows;
pub use windows::*;

mod tray;
pub use tray::*;

mod state;
pub use state::*;

mod history; 
pub use history::*;

mod keybind;
pub use keybind::*;

mod extensions;
pub use extensions::*;

mod settings;
pub use settings::*;

mod parser;
pub use parser::*;

mod autostart;
pub use autostart::*;

const INSTANCE_LOCK: &str = "lavendeux-instance-lock";

#[cfg(debug_assertions)]
const DEFAULT_LOGLEVEL: logs::LogLevel = logs::LogLevel::Debug;

#[cfg(not(debug_assertions))]
const DEFAULT_LOGLEVEL: logs::LogLevel = logs::LogLevel::Error;

/// Setup tauri app
fn setup_app() -> tauri::App {
	tauri::Builder::default()
	// Setup application state
		.manage(SharedState(Mutex::new(state.clone())))
	// Setup main window menu
		.menu(MainWindow::get_menu())
		.on_menu_event(MainWindow::handle_menu_event)
	// Setup system tray
		.system_tray(Tray::new_tray([].to_vec()))
		.on_system_tray_event(Tray::handle_event)
	// Add handlers
		.invoke_handler(tauri::generate_handler![
			settings::update_settings, settings::format_shortcut,
			windows::hide_errorwindow, history::clear_history, windows::show_history_tab,
			extensions::import_extension, extensions::disable_extension, 
			extensions::reload_all_extensions, extensions::open_extensions_dir,
			logs::get_logs
		])
		.build(tauri::generate_context!())
		.expect("error while running tauri application")
}

// Ensure single instance
fn instance_lock() -> SingleInstance {
	let instance = SingleInstance::new(INSTANCE_LOCK).unwrap();
	if !instance.is_single() {
		let w = MainWindow::new(app.handle()).unwrap();
		w.hide().ok();
		tauri::api::notification::Notification::new(&app.config().tauri.bundle.identifier)
			.title("Already Running!")
			.body("Lavendeux is already running - check running processes")
			.show().ok();

		state.logger.debug(&app.handle(), "Existing instance found! Exiting.");

		let handle = app.handle();
		std::thread::spawn(move || {
            thread::sleep(time::Duration::from_millis(100));
			handle.exit(0);
		});
	}
}

/// Process a commandline argument
fn process_argument(argument: &str, handler: Fn(&str)) {
	let arg_match = matches.args.get("config").unwrap();
	if arg_match.occurrences > 0 {
		handler(config_path.value.as_str().unwrap());
	}
}

fn main() {
	// Initialize base state
	let config = settings::Settings::new();
	let mut state = State {
		logger: logs::LogManager::new(config.logname.as_str(), DEFAULT_LOGLEVEL),
		parser: ParserState::new(),
		settings: config.clone(),
		history: Vec::new(),
	};

	// Configure tauri app
	let app = setup_app();
	
	// Instance lock
	instance_lock();
		
	app.run(move |app_handle, e| match e {
		// Runs when the application starts
		RunEvent::Ready => {
			let app_handle = app_handle.clone();
			let settings_ = state.clone().settings;
			let mut_state : tauri::State<SharedState> = app_handle.state();

			// Read commandline arguments
			if let Ok(mut lock) = mut_state.0.lock() {
				let matches = get_matches(app_handle.config().tauri.cli.as_ref().unwrap(), app_handle.package_info()).unwrap();

				// Configuration path
				process_argument("config", |filename| {
				lock.settings.filename = filename.to_string();
					if let Ok(new_settings) = read_settings(Some(filename)) {
						lock.settings = new_settings
					}
				});

				// Debug level
				process_argument("log-level", |loglevel| {
					match loglevel.to_lowercase().as_str() {
						"silly" => lock.logger.set_level(LogLevel::Silly),
						"debug" => lock.logger.set_level(LogLevel::Debug),
						"warning" => lock.logger.set_level(LogLevel::Warning),
						"error" => lock.logger.set_level(LogLevel::Error),
						"critical" => lock.logger.set_level(LogLevel::Critical),
						_ => {}
					}
				});
				
				lock.logger.debug(&app_handle, "Started Lavendeux");
			}

			// Await ready signal
			let app_handle_ = app_handle.clone();
			app_handle.listen_global("ready", move |_| {
				// Prepare error window
				match ErrorWindow::new(app_handle_.clone()) {
					Some(w) => w.show_message(
						"Ready to go!", 
						&format!("Solve highlighted equations with {}", settings::shortcut_name(&settings_)), 
						"success"
					).ok(),
					None => Some(())
				};
	
				// Apply default settings
				match settings::update_settings(app_handle_.clone(), app_handle_.clone().state(), settings_.clone()) {
					Ok(_) => {},
					Err(e) => println!("{}", e)
				};
			});
		},

		// Prevent window closing
		RunEvent::WindowEvent { label, event, .. } => {
			match event {
				WindowEvent::CloseRequested { api, .. } => {
					if label == "main" {
						let app_handle = app_handle.clone();
						let window = app_handle.get_window(&label).unwrap();
						api.prevent_close();
						
						thread::spawn(move || {
							window.hide().ok();
						});
					}
				},
				_ => {}
			};
		},

		// Keep the event loop running even if all windows are closed
		// This allow us to catch system tray events when there is no window
		RunEvent::ExitRequested { api, .. } => {
			api.prevent_exit();
		},

		_ => {}
	});
}
