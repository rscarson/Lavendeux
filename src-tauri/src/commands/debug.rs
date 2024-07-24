use crate::{
    controllers::{Controller, DebugController},
    error::Error,
    models::debug::DebugOutput,
};
use tauri::AppHandle;

#[tauri::command]
pub fn read_debug(app: AppHandle) -> Result<DebugOutput, Error> {
    DebugController(app).read().map(|d| d.output)
}

#[tauri::command]
pub fn activate_debug(app: AppHandle) -> Result<(), Error> {
    DebugController(app.clone()).activate()
}
