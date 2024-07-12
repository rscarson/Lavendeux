use tauri::AppHandle;

use crate::{
    controllers::{ExtensionsController, SettingsController},
    models::extension::Extensions,
};

#[tauri::command]
pub fn open_ext_dir(app: AppHandle) {
    SettingsController(app).open_ext_dir()
}

#[tauri::command]
pub fn read_extensions(app: AppHandle) -> Extensions {
    ExtensionsController(app).list()
}

#[tauri::command]
pub fn add_extension(filename: String, app: AppHandle) -> Result<Extensions, ()> {
    let controller = ExtensionsController(app);
    controller.add(&filename).map_err(|_| ())?;
    Ok(controller.list())
}

#[tauri::command]
pub fn del_extension(id: usize, app: AppHandle) -> Result<Extensions, ()> {
    let controller = ExtensionsController(app);
    controller.remove(id);
    Ok(controller.list())
}
