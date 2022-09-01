use std::fs::{ OpenOptions };
use std::io::{ Write };
use std::path::Path;

use super::{ Entry, Level };

#[cfg(debug_assertions)]
const DEFAULT_LEVEL: Level = Level::Debug;

#[cfg(not(debug_assertions))]
const DEFAULT_LEVEL: Level = Level::Error;

/// Manages log entries
#[derive(Clone)]
pub struct Manager {
    target: Option<String>,
    level: Level,
    buffer: Vec<Entry>
}

impl Manager {
    /// Create a new log manager
    /// 
    /// # Arguments
    /// * `target` - Target filename
    pub fn new(target: Option<String>) -> Self {
        Self::new_with_level(target, DEFAULT_LEVEL)
    }

    /// Create a new log manager
    /// 
    /// # Arguments
    /// * `target` - Target filename
    /// * `level` - Logging level
    pub fn new_with_level(target: Option<String>, level: Level) -> Self {
        let buffer : Vec<Entry> = Vec::new();
        Self {
            target: target, 
            level, buffer
        }
    }

    pub fn clear(&mut self) {
        self.buffer.clear();
    }

    /// Change the Level for this manager
    /// 
    /// # Arguments
    /// * `target` - Target filename
    /// * `level` - Logging level
    pub fn set_level(&mut self, level: Level) {
        self.level = level;
    }

    /// Return target file
    pub fn target(&self) -> Option<String> {
        self.target.clone()
    }

    /// Return log contents
    pub fn entries(&self) -> Vec<Entry> {
        self.buffer.clone()
    }

    /// Write an entry to the log
    /// 
    /// # Arguments
    /// * `text` - Event description
    /// * `level` - Logging level for the event
    pub fn log(&mut self, text: &str, level: Level) {
        if self.level.value() <= level.value() {
            let entry = Entry::new(text, level);
            let fmt = format!("{}    ({}) {}", entry.timestamp(), entry.level().to_string(), entry.text()); 
            self.buffer.push(entry);
            println!("{}", fmt);

            if let Some(target) = self.target() {
                let target_exists = Path::new(&target).exists();
                match OpenOptions::new().create(!target_exists).write(true).append(target_exists).open(target) {
                    Ok(mut f) => {
                        write!(f, "{}\n", fmt).ok();
                    },
                    Err(e) => {
                        println!("Could not write: {}", e);
                    }
                }
            }
        }
    }

    /// Write a debug entry to the log
    /// 
    /// # Arguments
    /// * `text` - Event description
    pub fn debug(&mut self, text: &str) {
        self.log(text, Level::Debug)
    }

    /// Write a warning entry to the log
    /// 
    /// # Arguments
    /// * `text` - Event description
    pub fn warning(&mut self, text: &str) {
        self.log(text, Level::Warning)
    }

    /// Write an error entry to the log
    /// 
    /// # Arguments
    /// * `text` - Event description
    pub fn error(&mut self, text: &str) {
        self.log(text, Level::Error)
    }
}