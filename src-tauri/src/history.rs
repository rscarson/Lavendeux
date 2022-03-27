use crate::state::SharedState;
use super::tray::Tray;
use tauri::{AppHandle, Manager};

const MAX_HISTORY_STR_LEN : usize = 50;
const TRAY_HISTORY_LEN : usize = 5;

/// Parser history entry
#[derive(Clone, serde::Serialize)]
pub struct History {
    pub expression: String,
    pub result: Result<String, String>
}

impl std::fmt::Display for History {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
		let len = if self.expression.len() < MAX_HISTORY_STR_LEN {self.expression.len()} else {MAX_HISTORY_STR_LEN};
        write!(f, "{}", self.expression.clone()[..len].to_string())
    }
}

/// Clear current history
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn clear_history(app_handle: AppHandle, state: tauri::State<SharedState>) -> Option<String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			lock.history.clear();
			update_history(app_handle, lock.history.clone())
		},
		None => Some("Could not lock settings object".to_string())
	}

}

/// Update history
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
pub fn update_history(app_handle: AppHandle, history: Vec<History>) -> Option<String> {	
	// Update tray history
	let recent_history = history.iter().rev().take(TRAY_HISTORY_LEN).clone().into_iter().map(|h| h.to_string()).collect::<Vec<String>>();
	let tray = Tray::new(app_handle.tray_handle());
	tray.update_menu(recent_history);

	app_handle.emit_all("history", history).ok()?;
	None
}