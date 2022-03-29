#![cfg_attr(
	all(not(debug_assertions), target_os = "windows"),
	windows_subsystem = "windows"
)]

use std::{thread, time};
use tauri::api::cli::get_matches;
use tauri::{RunEvent, Manager};
use std::sync::Mutex;
use single_instance::SingleInstance;

extern crate lavendeux_parser;
use lavendeux_parser::ParserState;

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

fn main() {
	// Initialize base state
	let instance = SingleInstance::new("lavendeux-instance-lock").unwrap();
	let state = State {
		settings: settings::Settings::new(),
		parser: ParserState::new(),
		history: Vec::new(),
	};

	let app = tauri::Builder::default()
	// Setup application state
		.setup(|_app| {    
			Ok(())
		})
		.manage(SharedState(Mutex::new(state.clone())))
	// Setup system tray
		.system_tray(Tray::new_tray([].to_vec()))
		.on_system_tray_event(Tray::handle_event)
	// Add handlers
		.invoke_handler(tauri::generate_handler![
			settings::update_settings, 
			windows::hide_errorwindow, history::clear_history, windows::show_history_tab,
			extensions::import_extension, extensions::disable_extension, 
			extensions::reload_all_extensions, extensions::open_extensions_dir
		])
		.build(tauri::generate_context!())
		.expect("error while running tauri application");

	
	// Instance lock
	if !instance.is_single() {
		let w = MainWindow::new(app.handle()).unwrap();
		w.hide().ok();
		tauri::api::notification::Notification::new(&app.config().tauri.bundle.identifier)
			.title("Already Running!")
			.body("Lavendeux is already running - check running processes")
			.show().ok();
		let handle = app.handle();
		std::thread::spawn(move || {
            thread::sleep(time::Duration::from_millis(100));
			handle.exit(0);
		});
	}
		
	app.run(move |app_handle, e| match e {
		// Runs when the application starts
		RunEvent::Ready => {
			let app_handle = app_handle.clone();
			let settings_ = state.clone().settings;
			let mut_state : tauri::State<SharedState> = app_handle.state();

			// Read commandline arguments
			if let Ok(mut lock) = mut_state.0.lock() {
				let matches = get_matches(app_handle.config().tauri.cli.as_ref().unwrap(), app_handle.package_info()).unwrap();
				let config_path = matches.args.get("config").unwrap();
				if config_path.occurrences > 0 {
					let filename = config_path.value.as_str().unwrap();
					lock.settings.filename = filename.to_string();
					if let Ok(new_settings) = read_settings(Some(filename)) {
						lock.settings = new_settings
					}
				}
			}

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
		RunEvent::CloseRequested { label, api, .. } => {
			let app_handle = app_handle.clone();
			let window = app_handle.get_window(&label).unwrap();
			api.prevent_close();
			
			thread::spawn(move || {
				window.hide().ok();
			});
		},

		// Keep the event loop running even if all windows are closed
		// This allow us to catch system tray events when there is no window
		RunEvent::ExitRequested { api, .. } => {
			api.prevent_exit();
		},

		_ => {}
	});
}
