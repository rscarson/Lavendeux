use tauri::AppHandle;

use crate::{
    controllers::{BlacklistController, Controller, DebugableResult, ExtensionsController},
    models::extension::Blacklist,
};

#[tauri::command]
pub fn read_blacklist(app: AppHandle) -> Result<Blacklist, ()> {
    BlacklistController(app).read().ok_or(())
}

#[tauri::command]
pub fn disable_extension(filename: String, app: AppHandle) -> Result<Blacklist, ()> {
    BlacklistController(app).disable(filename).ok_or(())
}

#[tauri::command]
pub fn enable_extension(filename: String, app: AppHandle) -> Result<Blacklist, ()> {
    let r = BlacklistController(app.clone()).enable(filename).ok_or(());
    ExtensionsController(app.clone())
        .reload()
        .debug_ok(&app, "reload-extensions");
    r
}
