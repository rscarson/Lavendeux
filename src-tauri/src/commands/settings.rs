use tauri::AppHandle;

use crate::{
    controllers::{ConfigController, Controller, SettingsController},
    models::settings::Settings,
};

#[tauri::command]
pub fn open_config_dir(app: AppHandle) {
    ConfigController(app).open_config_dir()
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
