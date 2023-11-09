use tauri::AppHandle;

use crate::{
    config,
    controllers::{Controller, ExtensionsController},
    models::extension::Extensions,
};

#[tauri::command]
pub fn open_ext_dir(app: AppHandle) {
    config::ConfigManager::open_ext_dir(app)
}

#[tauri::command]
pub fn reload_extensions(app: AppHandle) -> Result<Extensions, ()> {
    ExtensionsController(app).reload().or(Err(()))
}

#[tauri::command]
pub fn read_extensions(app: AppHandle) -> Result<Extensions, ()> {
    ExtensionsController(app).read().ok_or(())
}

#[tauri::command]
pub fn add_extension(filename: String, app: AppHandle) -> Result<Extensions, ()> {
    ExtensionsController(app).add(&filename).ok_or(())
}

#[tauri::command]
pub fn del_extension(filename: String, app: AppHandle) -> Result<Extensions, ()> {
    ExtensionsController(app).remove(&filename).ok_or(())
}
