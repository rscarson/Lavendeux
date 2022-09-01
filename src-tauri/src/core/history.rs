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
/// * `history` - History contents
pub fn clear(history: &mut Vec<History>) {
	history.clear();
}

/// Get most recent history items
/// 
/// # Arguments
/// * `history` - History contents
pub fn recent(history: &Vec<History>) -> Vec<String> {
	history.iter().rev().take(TRAY_HISTORY_LEN).clone().into_iter().map(|h| h.to_string()).collect::<Vec<String>>()
}

#[cfg(test)]
mod test_core_history {
	use super::*;

	fn get_history() -> Vec<History> {
		let mut history = vec![];
		for i in 1..TRAY_HISTORY_LEN*2 {
			history.push(
				History::new(format!("e{}", i), Ok(format!("r{}", i)))
			)
		}
		history
	}

	#[test]
	fn test_history_new() {
		let history = History::new("expression".to_string(), Ok("result".to_string()));
        assert_eq!(history.expression, "expression".to_string());
        assert_eq!(true, history.result.is_ok());
		assert_eq!(false, history.timestamp.contains("-"));

	}

	#[test]
	fn test_clear() {
		let mut history = get_history();
		clear(&mut history);
        assert_eq!(history.len(), 0);
    }

    #[test]
    fn test_recent() {
		let mut history = get_history();
		let recent = recent(&mut history);

        assert_eq!(recent.len(), TRAY_HISTORY_LEN);
        assert_eq!(recent[0], history[history.len()-1].expression);
    }
}