use crate::core::State;
use time::{ OffsetDateTime, format_description::well_known::Rfc2822 };

const MAX_HISTORY_STR_LEN : usize = 50;
const TRAY_HISTORY_LEN : usize = 5;

/// Parser history entry
#[derive(Clone, serde::Serialize)]
pub struct History {
    pub expression: String,
	pub timestamp: String,
    pub result: Result<String, String>
}

impl std::fmt::Display for History {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
		let len = if self.expression.len() < MAX_HISTORY_STR_LEN {self.expression.len()} else {MAX_HISTORY_STR_LEN};
        write!(f, "{}", self.expression.clone()[..len].to_string())
    }
}

impl History {
	pub fn new(expression: String, result: Result<String, String>) -> Self {
		let mut h = Self {
			expression: expression,
			result: result,
			timestamp: OffsetDateTime::now_utc()
			.format(&Rfc2822)
			.ok().ok_or("----------------------------").unwrap()
		};
		h.timestamp = h.timestamp.drain(..(h.timestamp.len() - 6)).collect();
		h
	}
}

/// Clear current history
/// 
/// # Arguments
/// * `state` - Current application state
pub fn clear(state: &mut State) {
	state.history.clear();
}

/// Get most recent history items
/// 
/// # Arguments
/// * `state` - Current application state
pub fn recent(state: &State) -> Vec<String> {
	state.history.iter().rev().take(TRAY_HISTORY_LEN).clone().into_iter().map(|h| h.to_string()).collect::<Vec<String>>()
}