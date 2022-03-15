use tauri::{AppHandle, Manager, Position, ClipboardManager};
use crate::keybind;
use crate::SharedState;
use std::{thread, time};
use std::error::Error;
use lavendeux_parser::Token;
use super::tray::Tray;

const ERROR_WINDOW_OFFSET : u32 = 3;
const ERROR_WINDOW_DELAY : u64 = 5000;

#[derive(Clone, serde::Serialize)]
struct MessagePayload {
	title: String,
	msg: String,
	variant: String
}

#[tauri::command]
pub fn hide_errorwindow(app_handle: AppHandle) {
	match app_handle.get_window("error") {
		Some(error_window) => { error_window.hide().ok(); },
		None => {}
	}
}

pub fn display_message(title: &str, msg: &str, payload: &str, app_handle: AppHandle) -> Option<tauri::Window> {
	// Get relevant handles
	let error_window = app_handle.get_window("error")?;
	let window_size = error_window.outer_size().ok()?;
	let monitor = error_window.current_monitor().ok()??;

	// Calculate position for the error window
	let mut pos = error_window.outer_position().ok()?;
	pos.x = (monitor.size().width - window_size.width - ERROR_WINDOW_OFFSET) as i32;
	pos.y = (monitor.size().height - window_size.height - ERROR_WINDOW_OFFSET) as i32;

	// Set window position and get focus
	app_handle.emit_all("message", MessagePayload {
		title: title.to_string(),
		msg: msg.to_string(), 
		variant: payload.to_string()
	}).ok()?;
	error_window.show().ok()?;
	error_window.set_position(Position::Physical(pos)).ok()?;
	error_window.set_always_on_top(true).ok()?;

	// Hide the window after a delay
	let w = error_window.clone();
	thread::spawn(move || {
		thread::sleep(time::Duration::from_millis(ERROR_WINDOW_DELAY));
		w.hide().ok();
	});

	Some(error_window)
}

pub fn do_parse(app_handle: AppHandle) -> Option<Box<dyn Error>> {
	let state : tauri::State<SharedState> = app_handle.state();
	let mut lock = state.0.lock().ok()?;

//	let mut clipboard_backup : String;
//	match app_handle.clipboard_manager().read_text().ok()? {
//		Some(s) => { clipboard_backup = s; },
//		None => { return Some(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "could not read from clipboard"))); }
//	}

	keybind::send_copy();
	match app_handle.clipboard_manager().read_text().ok()? {
		Some(input) => {
			match Token::from_input(&input, &mut lock.parser) {
				Ok(token) => {
					for line in token.children {
						lock.history.push(crate::History {
							expression: line.input.trim().to_string(),
							result: Ok(line.text)
						});
					}

					app_handle.clipboard_manager().write_text(token.text).ok()?;

					if lock.settings.auto_paste {
						keybind::send_paste();
					}
				
					app_handle.emit_all("history", lock.history.clone()).ok()?;
				},

				Err(e) => {
					if !lock.settings.silent_errors {
						display_message("Could not parse expression", &e.to_string(), "danger", app_handle.clone());
					}

					lock.history.push(crate::History {
						expression: input.clone(),
						result: Err(e.to_string())
					});
				}
			}
		},
		None => return Some(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "could not read from clipboard")))
	}


	let tray = Tray::new(app_handle.tray_handle());
	tray.update_menu(lock.history.iter().rev().take(10).clone().into_iter().map(|h| h.to_string()).collect::<Vec<String>>());

	app_handle.emit_all("history", lock.history.clone()).ok()?;
//	app_handle.clipboard_manager().write_text(clipboard_backup).ok()?;
	None
}

pub fn handle_shortcut(app_handle: AppHandle) {
	match do_parse(app_handle.clone()) {
		Some(e) => {
			display_message("Something went wrong!", &e.to_string(), "danger", app_handle.clone());
		},
		None => {}
	}
}

#[tauri::command]
pub fn clear_history(app_handle: AppHandle, state: tauri::State<SharedState>) -> Option<String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			lock.history.clear();
			app_handle.emit_all("history", lock.history.clone()).ok()?;
			None
		},

		None => Some("Could not lock settings object".to_string())
	}
}