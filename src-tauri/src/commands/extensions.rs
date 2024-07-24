use tauri::AppHandle;

use crate::{
    controllers::{ExtensionsController, SettingsController},
    error::Error,
    models::extension::Extensions,
};

#[tauri::command]
pub fn open_ext_dir(app: AppHandle) -> Result<(), Error> {
    SettingsController(app.clone()).open_ext_dir()
}

#[tauri::command]
pub fn read_extensions(app: AppHandle) -> Extensions {
    ExtensionsController(app).list()
}

#[tauri::command]
pub fn add_extension(filename: String, app: AppHandle) -> Result<Extensions, Error> {
    let controller = ExtensionsController(app);
    controller.add(&filename)?;
    Ok(controller.list())
}

#[tauri::command]
pub fn del_extension(id: usize, app: AppHandle) -> Result<Extensions, Error> {
    let controller = ExtensionsController(app);
    controller.remove(id)?;
    Ok(controller.list())
}
