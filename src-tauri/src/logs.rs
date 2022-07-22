use std::fs::{ OpenOptions };
use std::io::{ Write };
use tauri::{ AppHandle, Manager };
use time::{ OffsetDateTime, format_description::well_known::Rfc2822 };
use crate::state::SharedState;

/// Log levels
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub enum LogLevel {
	Silly, Debug, Info, Warning, Error, Critical
}
impl LogLevel {
    pub fn value(&self) -> i32 {
        match self {
            Self::Silly => -1,
            Self::Debug => 0,
            Self::Info => 1,
            Self::Warning => 2,
            Self::Error => 3,
            Self::Critical => 4,
        }
    }

    pub fn to_string(&self) -> String {
        match self {
            Self::Silly => "Silly".to_string(),
            Self::Debug => "Debug".to_string(),
            Self::Info => "Info".to_string(),
            Self::Warning => "Warning".to_string(),
            Self::Error => "Error".to_string(),
            Self::Critical => "Critical".to_string(),
        }
    }
}

/// A timestamped log entry
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub struct LogEntry {
    level: LogLevel,
    timestamp: String,
    text: String
}
impl LogEntry {
    /// Create a new log entry
    /// 
    /// # Arguments
    /// * `text` - Event description
    pub fn new(text: &str, level: LogLevel) -> Self {
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
    pub fn new_with_time(timestamp: String, level: LogLevel, text: &str) -> Self {
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
    pub fn level(&self) -> LogLevel {
        self.level.clone()
    }
}

/// Manages log entries
#[derive(Clone)]
pub struct LogManager {
    target: String,
    level: LogLevel,
    buffer: Vec<LogEntry>
}

impl LogManager {
    /// Create a new log manager
    /// 
    /// # Arguments
    /// * `target` - Target filename
    /// * `level` - Logging level
    pub fn new(target: &str, level: LogLevel) -> Self {
        let buffer : Vec<LogEntry> = Vec::new();
        Self {
            target: target.to_string(), 
            level, buffer
        }
    }

    /// Change the loglevel for this manager
    /// 
    /// # Arguments
    /// * `target` - Target filename
    /// * `level` - Logging level
    pub fn set_level(&mut self, level: LogLevel) {
        self.level = level;
    }

    /// Return target file
    pub fn target(&self) -> String {
        self.target.clone()
    }

    /// Return log contents
    pub fn entries(&self) -> Vec<LogEntry> {
        self.buffer.clone()
    }

    /// Write an entry to the log
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `text` - Event description
    /// * `level` - Logging level for the event
    pub fn log(&mut self, app_handle: &AppHandle, text: &str, level: LogLevel) {
        if self.level.value() <= level.value() {
            let entry = LogEntry::new(text, level);
            let fmt = format!("{}    ({}) {}", entry.timestamp(), entry.level().to_string(), entry.text()); 
            self.buffer.push(entry);
            println!("{}", fmt);

            if let Ok(mut f) = OpenOptions::new()
                .append(true)
                .open(self.target()) {
            
                write!(f, "{}\n", fmt).ok();
            }

			app_handle.emit_all("logs", self.entries()).unwrap();
        }
    }

    /// Write a debug entry to the log
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `text` - Event description
    pub fn debug(&mut self, app_handle: &AppHandle, text: &str) {
        self.log(app_handle, text, LogLevel::Debug)
    }

    /// Write a warning entry to the log
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `text` - Event description
    pub fn warning(&mut self, app_handle: &AppHandle, text: &str) {
        self.log(app_handle, text, LogLevel::Warning)
    }

    /// Write an error entry to the log
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `text` - Event description
    pub fn error(&mut self, app_handle: &AppHandle, text: &str) {
        self.log(app_handle, text, LogLevel::Error)
    }
}

/// Get the latest log entries
/// 
/// # Arguments
/// * `app_handle` - Application handle
/// * `state` - Application state
#[tauri::command]
pub fn get_logs(state: tauri::State<SharedState>) -> Result<Vec<LogEntry>, String> {
	match state.0.lock().ok() {
		Some(lock) => {
			Ok(lock.logger.entries())
		},

		None => Err("Could not lock settings object".to_string())
	}
}