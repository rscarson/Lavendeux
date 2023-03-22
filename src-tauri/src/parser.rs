use tauri::{AppHandle, Manager};
use lavendeux_parser::Token;

use crate::{
	ui::{ tray, keyboard, windows, clipboard },
	core::{ State, history	}
};

const MAX_HISTORY_LEN : usize = 50;

/// Handle a keyboard-shortcut for parsing
/// 
/// # Arguments
/// * `app_handle` - AppHandle
pub fn handle_shortcut(app_handle: &AppHandle, state: &mut State) {
	if let Err(e) = do_parse(app_handle, state) {
		match windows::Error::new(app_handle.clone()) {
			Some(w) => w.show_message("Something went wrong!", &e.to_string(), "danger").ok(),
			None => Some(())
		};
	}
}

/// Notify front-end of history changes
/// 
/// # Arguments
/// * `app_handle` - Application handle
/// * `state` - Application state
pub fn notify(app_handle: &AppHandle, state: &State) {
	// Notify front-end
	let recent_history = history::recent(&state.history);
	let tray = tray::Tray::new(app_handle.tray_handle());
	tray.update_menu(recent_history);
	app_handle.emit_all("history", state.history.clone()).ok();
}

/// Attempt to parse an expression
/// 
/// # Arguments
/// * `expression` - input to parse
/// * `app_handle` - AppHandle
pub fn parse_expression(expression: &str, app_handle: &AppHandle, state: &mut State) -> Result<String, String> {
	match Token::new(expression, &mut state.parser) {
		Ok(token) => {
			// Got a result - add it to history
			for line in token.children() {
				state.history.push(history::History::new(
					line.input().trim().to_string(),
					Ok(line.text().to_string())
				));
				if state.history.len() > MAX_HISTORY_LEN {
					state.history.remove(0);
				}
			}

			Ok(token.text().to_string())
		},

		Err(e) => {
			// Error parsing - display an error
			if !state.settings.inner_settings.silent_errors {
				if let Some(error) = windows::Error::new(app_handle.clone()) {
					error.show_message("Could not parse expression", &e.to_string(), "danger").ok();
				}
			}

			// Push to history
			state.history.push(history::History::new(
				expression.to_string(),
				Err(e.to_string())
			));

			Err(e.to_string())
		}
	}
}

/// Parse clipboard as expression
/// 
/// # Arguments
/// * `app_handle` - AppHandle
fn do_parse(app_handle: &AppHandle, state: &mut State) -> Result<(), String> {
	// Fill the clipboard, after backing it up
	let clip_backup = clipboard::read(app_handle)?;
	if state.settings.inner_settings.auto_paste {
		keyboard::send_copy();
	}

	let transform_result = clipboard::transform(app_handle, state, |input, state| {
		parse_expression(&input, app_handle, state)
	}); 
	notify(app_handle, state);

	// Reset clipboard contents
	if state.settings.inner_settings.auto_paste {
		keyboard::send_paste();
		clipboard::write(app_handle, &clip_backup)?;
	}
	
	// Notify front-end
	let recent_history = history::recent(&state.history);
	let tray = tray::Tray::new(app_handle.tray_handle());
	tray.update_menu(recent_history);
	app_handle.emit_all("history", state.history.clone()).ok();

	if let Err(_) = transform_result {
		let error = "Unable to read from the clipboard - it may be locked by another application";
		state.logger.error(error);
		Err(error.to_string())
	} else {
		Ok(())
	}
}