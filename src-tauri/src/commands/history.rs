use std::{fs::File, io::Write, path::Path};

use tauri::AppHandle;

use crate::{
    controllers::{Controller, HistoryController},
    models::history::History,
};

#[tauri::command]
pub fn read_history(app: AppHandle) -> Option<History> {
    HistoryController(app).read()
}

#[tauri::command]
pub fn del_history(id: usize, app: AppHandle) {
    HistoryController(app).remove(id);
}

#[tauri::command]
pub fn export_history(destination: &Path, app: AppHandle) -> Result<(), String> {
    if let Some(history) = HistoryController(app).read() {
        let s = serde_json::to_string_pretty(&history.clone()).or_else(|e| Err(e.to_string()))?;
        let mut file = File::create(destination).or_else(|e| Err(e.to_string()))?;
        file.write_all(s.as_bytes())
            .or_else(|e| Err(e.to_string()))?;
    }

    Ok(())
}

#[tauri::command]
pub fn clear_history(app: AppHandle) {
    HistoryController(app).clear();
}
