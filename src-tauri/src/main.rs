#![cfg_attr(
	all(not(debug_assertions), target_os = "windows"),
	windows_subsystem = "windows"
)]

use tauri::{RunEvent, Manager};
use std::sync::Mutex;

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
	let state = State {
		settings: settings::Settings::new(),
		parser: ParserState::new(),
		history: Vec::new(),
	};

	let app = tauri::Builder::default()
	// Setup application state
		.setup(|_app| { Ok(()) })
		.manage(SharedState(Mutex::new(state.clone())))
	// Setup system tray
		.system_tray(Tray::new_tray([].to_vec()))
		.on_system_tray_event(Tray::handle_event)
	// Add menu to main window
		.menu(MainWindow::get_menu())
		.on_menu_event(MainWindow::handle_menu_event)
	// Add handlers
		.invoke_handler(tauri::generate_handler![
			settings::update_settings, 
			windows::hide_errorwindow, history::clear_history,
			extensions::import_extension, extensions::disable_extension, 
			extensions::reload_all_extensions, extensions::open_extensions_dir
		])
		.build(tauri::generate_context!())
		.expect("error while running tauri application");
		
	app.run(move |app_handle, e| match e {
		// Runs when the application starts
		RunEvent::Ready => {
			let settings_ = state.clone().settings.clone();
			let app_handle = app_handle.clone();

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
			window.hide().ok();
		},

		// Keep the event loop running even if all windows are closed
		// This allow us to catch system tray events when there is no window
		RunEvent::ExitRequested { api, .. } => {
			api.prevent_exit();
		},

		_ => {}
	});
}
