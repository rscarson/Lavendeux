use time::{ OffsetDateTime, format_description::well_known::Rfc2822 };

use super::Level;

/// A timestamped log entry
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub struct Entry {
    level: Level,
    timestamp: String,
    text: String
}
impl Entry {
    /// Create a new log entry
    /// 
    /// # Arguments
    /// * `text` - Event description
    pub fn new(text: &str, level: Level) -> Self {
        Self::new_with_time(
            OffsetDateTime::now_utc()
                .format(&Rfc2822)
                .ok().ok_or("----------------------------").unwrap(), 
            level, text
        )
    }

    /// Create a new log entry
    /// 
    /// # Arguments
    /// * `timestamp` - Event time
    /// * `text` - Event description
    pub fn new_with_time(timestamp: String, level: Level, text: &str) -> Self {
        Self {
            timestamp, level,
            text: text.to_string()
        }
    }

    /// Return event time
    pub fn timestamp(&self) -> String {
        self.timestamp.clone()
    }

    // Return event text
    pub fn text(&self) -> String {
        self.text.clone()
    }

    // Return event level
    pub fn level(&self) -> Level {
        self.level.clone()
    }
}