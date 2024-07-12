use tauri::AppHandle;

use crate::{
    controllers::{Controller, DebugableResult, ParserController, SettingsController},
    models::settings::Settings,
};

#[tauri::command]
pub fn open_config_dir(app: AppHandle) {
    SettingsController(app).open_config_dir()
}

#[tauri::command]
pub fn read_settings(app: AppHandle) -> Result<Settings, ()> {
    SettingsController(app).read().ok_or(())
}

#[tauri::command]
pub fn write_settings(settings: Settings, app: AppHandle) -> Result<(), String> {
    SettingsController(app).write(&settings).and(Ok(()))
}

#[tauri::command]
pub fn app_exit(app: AppHandle) {
    app.exit(0);
}

#[tauri::command]
pub fn restart_parser(app: AppHandle) {
    ParserController(app.clone())
        .restart_parser()
        .debug_ok(&app, "restart-parser");
}
