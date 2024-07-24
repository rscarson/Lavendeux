use std::{fs::File, io::Write, path::Path};

use tauri::AppHandle;

use crate::{
    controllers::{Controller, HistoryController},
    error::Error,
    models::history::History,
};

#[tauri::command]
pub fn read_history(app: AppHandle) -> Result<History, Error> {
    HistoryController(app).read()
}

#[tauri::command]
pub fn del_history(id: usize, app: AppHandle) -> Result<(), Error> {
    HistoryController(app).remove(id)
}

#[tauri::command]
pub fn export_history(destination: &Path, app: AppHandle) -> Result<(), Error> {
    let s = serde_json::to_string_pretty(&HistoryController(app).read()?)?;
    let mut file = File::create(destination)?;
    file.write_all(s.as_bytes())?;

    Ok(())
}

#[tauri::command]
pub fn clear_history(app: AppHandle) -> Result<(), Error> {
    HistoryController(app).clear()
}
