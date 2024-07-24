use tauri::AppHandle;

use crate::{
    bugcheck::BugcheckedOk,
    controllers::{Controller, ParserController, SettingsController},
    error::Error,
    models::settings::Settings,
};

#[tauri::command]
pub fn open_config_dir(app: AppHandle) -> Result<(), Error> {
    SettingsController(app.clone()).open_config_dir()
}

#[tauri::command]
pub fn read_settings(app: AppHandle) -> Result<Settings, Error> {
    SettingsController(app).read()
}

#[tauri::command]
pub fn write_settings(settings: Settings, app: AppHandle) -> Result<(), Error> {
    SettingsController(app).write(&settings)?;
    Ok(())
}

#[tauri::command]
pub fn app_exit(app: AppHandle) {
    app.exit(0);
}

#[tauri::command]
pub fn restart_parser(app: AppHandle) {
    ParserController(app.clone())
        .restart_parser()
        .checked_ok(&app, "Error restarting parser");
}
