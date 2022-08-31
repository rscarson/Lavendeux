use tauri::{AppHandle, ClipboardManager};

use crate::core::State;

const CLIPBOARD_RETRIES : usize = 3;

/// Try several times to read from the clipboard
/// 
/// # Arguments
/// * `app_handle` - AppHandle
pub fn read(app_handle: &AppHandle) -> Result<String, String> {
    let mut last_error: String = "".to_string();
	for _ in 0..CLIPBOARD_RETRIES {
        match app_handle.clipboard_manager().read_text() {
            Ok(s) => return Ok(s.unwrap_or_default()),
            Err(e) => last_error = e.to_string()
        }
	}
    Err(last_error)
}

/// Try several times to write to the clipboard
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `text` - Text to write out
pub fn write(app_handle: &AppHandle, text: &str) -> Result<(), String> {
    let mut last_error: String = "".to_string();
	for _ in 0..CLIPBOARD_RETRIES {
        match app_handle.clipboard_manager().write_text(text) {
            Ok(_) => return Ok(()),
            Err(e) => last_error = e.to_string()
        }
	}
    Err(last_error)
}

/// Transform the contents of the clipboard
/// 
/// # Arguments
/// * `app_handle` - App handle
/// * `state` - Application state
/// * `handler` - Handler that wil transform the contents
pub fn transform<F>(app_handle: &AppHandle, state: &mut State, handler: F) -> Result<String, String>
where F: Fn(&str, &mut State) -> Result<String, String> {
	let contents = read(app_handle)?;
    if let Ok(result) = handler(&contents, state) {
        write(app_handle, &result)?;
        Ok(result)
    } else {
        Ok(contents)
    }
}