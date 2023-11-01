use core::fmt;
use serde::{Deserialize, Serialize};
use std::{fs::File, io::Write, path::Path, time::SystemTime};
use tauri::{AppHandle, Manager, State};

use crate::ManagedValue;

use super::tray::update_tray;

#[derive(Deserialize, Serialize, Clone, Debug)]
pub enum SnippetResult {
    Success(String),
    Error(String),
}

impl fmt::Display for SnippetResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Success(e) => write!(f, "{}", e),
            Self::Error(e) => write!(f, "{}", e),
        }
    }
}

#[derive(Deserialize, Serialize, Clone, Debug)]
pub struct Snippet {
    pub input: String,
    pub result: SnippetResult,
    pub timestamp: SystemTime,
}

impl Default for Snippet {
    fn default() -> Self {
        Self {
            input: "".to_string(),
            result: SnippetResult::Error("".to_string()),
            timestamp: SystemTime::now(),
        }
    }
}

impl Snippet {
    pub fn new(input: &str, result: SnippetResult) -> Self {
        Self {
            input: input.to_string(),
            result,
            timestamp: SystemTime::now(),
        }
    }

    pub fn is_err(&self) -> bool {
        matches!(self.result, SnippetResult::Error(_))
    }
}

pub type History = Vec<Snippet>;
type ManagedHistory = ManagedValue<History>;

static HISTORY_UPDATED_EVENT: &'static str = "updated-history";
static HISTORY_MAX_LENGTH: usize = 50;

struct HistoryManager();
impl HistoryManager {
    ///
    /// Empty the history and return the result if successful
    pub fn clear(state: &ManagedHistory) -> Option<History> {
        if let Some(mut history) = state.read() {
            history.clear();
            return Some(history.to_vec());
        };
        None
    }

    ///
    /// Remove an item from the history and return the result if successful
    pub fn remove(id: usize, state: &ManagedHistory) -> Option<History> {
        if let Some(mut history) = state.read() {
            if history.get(id).is_some() {
                history.remove(id);
                return Some(history.to_vec());
            }
        }
        None
    }

    ///
    /// Try to read the history
    pub fn read(state: &ManagedHistory) -> Option<History> {
        state.clone_inner()
    }

    ///
    /// Add an item to the history tab
    pub fn add(snippet: Snippet, state: &ManagedHistory) -> Option<History> {
        if let Some(mut history) = state.read() {
            history.insert(0, snippet);
            history.truncate(HISTORY_MAX_LENGTH);
            return Some(history.to_vec());
        };
        None
    }
}

pub fn append_history(snippet: Snippet, app: AppHandle) {
    let state = app.state::<ManagedHistory>();
    if let Some(history) = HistoryManager::add(snippet, state.inner()) {
        update_tray(&app).ok();
        app.emit(HISTORY_UPDATED_EVENT, history.clone()).ok();
    }
}

#[tauri::command]
pub fn read_history(state: State<ManagedHistory>) -> Option<History> {
    HistoryManager::read(state.inner())
}

#[tauri::command]
pub fn del_history(id: usize, state: State<ManagedHistory>, app: AppHandle) {
    if let Some(history) = HistoryManager::remove(id, state.inner()) {
        update_tray(&app).ok();
        app.emit(HISTORY_UPDATED_EVENT, history.clone()).ok();
    }
}

#[tauri::command]
pub fn export_history(destination: &Path, state: State<ManagedHistory>) -> Result<(), String> {
    if let Some(history) = read_history(state) {
        let s = serde_json::to_string_pretty(&history.clone()).or_else(|e| Err(e.to_string()))?;
        let mut file = File::create(destination).or_else(|e| Err(e.to_string()))?;
        file.write_all(s.as_bytes())
            .or_else(|e| Err(e.to_string()))?;
    }

    Ok(())
}

#[tauri::command]
pub fn clear_history(state: State<ManagedHistory>, app: AppHandle) {
    if let Some(history) = HistoryManager::clear(state.inner()) {
        update_tray(&app).ok();
        app.emit(HISTORY_UPDATED_EVENT, history.clone()).ok();
    }
}
