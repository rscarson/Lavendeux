use tauri::{AppHandle, Manager, ClipboardManager};
use crate::keybind;
use crate::SharedState;
use std::error::Error;
use lavendeux_parser::Token;
use crate::windows::ErrorWindow;
use crate::history::{History, update_history};

const MAX_HISTORY_LEN : usize = 50;
const CLIPBOARD_RETRIES : usize = 3;

/// Try several times to read from the clipboard
/// 
/// # Arguments
/// * `app_handle` - AppHandle
pub fn clipboard_retry_read(app_handle: &AppHandle) -> Option<Option<String>> {
	for _ in 0..CLIPBOARD_RETRIES {
		if let Ok(o) = app_handle.clipboard_manager().read_text() {
			return Some(o);
		}
	}
	None
}

/// Try several times to write to the clipboard
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `text` - Text to write out
pub fn clipboard_retry_write(app_handle: &AppHandle, text: &str) -> Result<(), ()> {
	for _ in 0..CLIPBOARD_RETRIES {
		if app_handle.clipboard_manager().write_text(text).is_ok() {
			return Ok(());
		}
	}
	Err(())
}

/// Attempt to parse an expression
/// 
/// # Arguments
/// * `app_handle` - AppHandle
pub fn do_parse(app_handle: AppHandle) -> Option<Box<dyn Error>> {
	// Get current state
	let state : tauri::State<SharedState> = app_handle.state();
	let mut lock = state.0.lock().ok()?;

	// Fill the clipboard
	if lock.settings.auto_paste {
		keybind::send_copy();
	}

	// Read from the clipboard
	match clipboard_retry_read(&app_handle)? {
		Some(input) => {
			match Token::new(&input, &mut lock.parser) {
				Ok(token) => {
					// Got a result - add it to history
					for line in token.children() {
						lock.history.push(History::new(
							line.input().trim().to_string(),
							Ok(line.text().to_string())
						));
						if lock.history.len() > MAX_HISTORY_LEN {
							lock.history.remove(0);
						}
					}

					// Put the result in the clipboard
					clipboard_retry_write(&app_handle, &token.text()).ok()?;
					if lock.settings.auto_paste {
						keybind::send_paste();
					}
					
					// Notify front-end
					app_handle.emit_all("history", lock.history.clone()).ok()?;
				},

				Err(e) => {
					// Error parsing - display an error
					if !lock.settings.silent_errors {
						ErrorWindow::new(app_handle.clone())?.show_message("Could not parse expression", &e.to_string(), "danger").ok()?;
					}

					// Push to history
					lock.history.push(History::new(
						input.clone(),
						Err(e.to_string())
					));
				}
			}
		},

		// Could not read the clipboard
		None => {
			let state: tauri::State<SharedState> = app_handle.state();
			if let Ok(mut lock) = state.0.lock() {
				lock.logger.error(&app_handle, "Unable to read from the clipboard - it may be locked by another application");
			}
			
			return Some(
				Box::new(
					std::io::Error::new(std::io::ErrorKind::Other, "could not read from clipboard")
				)
			)
		}
	}

	// Update tray history
	update_history(app_handle.clone(), lock.history.clone());

	None
}

/// Handle a keyboard-shortcut for parsing
/// 
/// # Arguments
/// * `app_handle` - AppHandle
pub fn handle_shortcut(app_handle: AppHandle) {
	if let Some(e) = do_parse(app_handle.clone()) {
		match ErrorWindow::new(app_handle) {
			Some(w) => w.show_message("Something went wrong!", &e.to_string(), "danger").ok(),
			None => Some(())
		};
	}
}