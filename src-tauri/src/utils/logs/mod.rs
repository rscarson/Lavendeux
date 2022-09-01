use crate::core::State;
use crate::utils::fs;

mod manager;
pub use manager::Manager;

mod level;
pub use level::Level;

mod entry;
pub use entry::Entry;

/// Open the log directory in the system explorer
/// 
/// # Arguments
/// * `state` - Current application state
pub fn open_logs_dir(state: &mut State) {
	if let Some(target) = state.logger.target() {
		if let Err(e) = fs::open(target) {
			state.logger.error(&format!("Error opening log directory: {}", e));
		}
	}
}

/// Open the log directory in the system explorer
/// 
/// # Arguments
/// * `state` - Current application state
pub fn clear_logs(state: &mut State) {
    state.logger.clear();
}

/// Get the latest log entries
/// 
/// # Arguments
/// * `state` - Current application state
pub fn get_logs(state: &mut State) -> Vec<Entry> {
	state.logger.entries()
}



