#![cfg_attr(
	all(not(debug_assertions), target_os = "windows"),
	windows_subsystem = "windows"
)]

use std::sync::Mutex;

extern crate lavendeux_parser;
use lavendeux_parser::{ParserState};

mod tray;
mod keybind;
mod extensions;
mod settings;
mod parser;
use tauri::{
	RunEvent, SystemTrayEvent, WindowMenuEvent,
	Menu, Submenu, CustomMenuItem, AppHandle,
	Manager, MenuItem, ClipboardManager
};

#[derive(Clone, serde::Serialize)]
pub struct History {
    pub expression: String,
    pub result: Result<String, String>
}

const MAX_HISTORY_STR_LEN : usize = 50;
impl History {
	fn to_string(&self) -> String {
		let len = if self.expression.len() < MAX_HISTORY_STR_LEN {self.expression.len()} else {MAX_HISTORY_STR_LEN};
		self.expression.clone()[..len].to_string()
	}
}

#[derive(Clone)]
pub struct State {
    pub parser: ParserState,
    pub settings: settings::Settings,
    pub history: Vec<History>
}

pub struct SharedState(Mutex<State>);

fn get_menu() -> Menu {
	Menu::new()
	.add_submenu(Submenu::new("File", Menu::new()
		.add_item(CustomMenuItem::new("clear_history", "Clear History"))
		.add_native_item(MenuItem::Separator)
		.add_item(CustomMenuItem::new("exit", "Exit"))
	))
	.add_submenu(Submenu::new("Help", Menu::new()
		.add_item(CustomMenuItem::new("about", "About"))
	))
}

fn handle_menu_event(e: WindowMenuEvent) {
	match e.menu_item_id() {
		"clear_history" => {
			let app = e.window().app_handle();
			parser::clear_history(app.clone(), app.state());
		},

		"about" => {
			tauri::api::dialog::message(Some(e.window()), "About Lavendeux", "Parse anywhere.\nDevelopped by @rscarson");
		},

		"exit" => {
			let app = e.window().app_handle();
			std::thread::spawn(move || {
				app.get_window("main").unwrap().close().unwrap();
			});

			//app.exit(0);
			std::process::exit(0);
		},

		_ => {}
	}
}

fn handle_tray_event(app: &AppHandle, e: SystemTrayEvent) {
	match e {
		SystemTrayEvent::DoubleClick {..} => {
			match app.get_window("main") {
				Some(w) => {
					w.show().ok();
				},
				None => return
			};
		},
		SystemTrayEvent::MenuItemClick {id, ..} => {
			match id.as_str() {
				"settings" => {
					match app.get_window("main") {
						Some(w) => {
							w.show().ok();
						},
						None => return
					};
				},

				"exit" => {
					let app_ = app.clone();
					std::thread::spawn(move || {
						app_.get_window("main").unwrap().close().unwrap();
					});

					//app.exit(0);
					std::process::exit(0);
				},

				_ => {
					match id.as_str().parse::<i32>() {
						Ok(n) => {
							let state : tauri::State<SharedState> = app.state();
							match state.0.lock().ok() {
								Some(lock) => {
									let item = &lock.history[lock.history.len() - 1 - (n as usize)];
									app.clipboard_manager().write_text(item.expression.clone()).ok();
								},
						
								None => { }
							};
						},
						Err(_) => { },
					}
				}
			}
		}

		_ => {}
	}
}

fn main() {
	let state = State {
		settings: settings::Settings::new(),
		parser: ParserState::new(),
		history: Vec::new(),
	};

	let app = tauri::Builder::default()
		.setup(|_app| {
			Ok(())
		})
		.manage(SharedState(Mutex::new(state.clone())))
		.system_tray(tray::Tray::new_tray())
		.on_system_tray_event(handle_tray_event)
		.menu(get_menu())
		.on_menu_event(handle_menu_event)
		.invoke_handler(tauri::generate_handler![
			settings::update_settings, settings::get_settings, 
			parser::hide_errorwindow, parser::clear_history,
			extensions::import_extension, extensions::disable_extension, extensions::reload_extensions,
			extensions::open_extensions_dir, 
		])
		.build(tauri::generate_context!())
		.expect("error while running tauri application");
		
	app.run(move |app_handle, e| match e {
		RunEvent::Ready => {
			let app_handle = app_handle.clone();
			let state = state.clone();
			let _tray = tray::Tray::new(app_handle.tray_handle().clone());

			let app_handle_ = app_handle.clone();
			let settings_ = state.settings.clone();
			app_handle.listen_global("ready", move |_| {
				let app_handle__ = app_handle_.clone();

				// Prepare error window
				parser::display_message(
					"Ready to go!", 
					&format!("Solve highlighted equations with {}", settings::shortcut_name(&settings_)), 
					"success", app_handle__.clone()
				);
	
				// Apply default settings
				match settings::update_settings(app_handle__.clone(), app_handle__.state(), settings_.clone()) {
					Ok(_) => {},
					Err(e) => println!("{}", e)
				};
			});
		},

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
		}

		_ => {}
	});
}
