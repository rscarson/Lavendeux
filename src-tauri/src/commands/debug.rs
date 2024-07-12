use tauri::AppHandle;

use crate::{
    controllers::{Controller, DebugController},
    models::debug::DebugOutput,
};

#[tauri::command]
pub fn read_debug(app: AppHandle) -> Result<DebugOutput, ()> {
    DebugController(app).read().map(|d| d.output).ok_or(())
}

#[tauri::command]
pub fn activate_debug(app: AppHandle) {
    DebugController(app).activate()
}
