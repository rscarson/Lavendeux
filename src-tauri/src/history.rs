use crate::state::SharedState;
use tauri::{AppHandle, Manager};

const MAX_HISTORY_STR_LEN : usize = 50;

/// Parser history entry
#[derive(Clone, serde::Serialize)]
pub struct History {
    pub expression: String,
    pub result: Result<String, String>
}

impl History {
	/// Convert the entry into a string of limited length
	pub fn to_string(&self) -> String {
		let len = if self.expression.len() < MAX_HISTORY_STR_LEN {self.expression.len()} else {MAX_HISTORY_STR_LEN};
		self.expression.clone()[..len].to_string()
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
			app_handle.emit_all("history", lock.history.clone()).ok()?;
			None
		},

		None => Some("Could not lock settings object".to_string())
	}
}