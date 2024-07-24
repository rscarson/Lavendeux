use tauri::AppHandle;

use crate::{controllers::LanguageController, error::Error, models::markdown::MarkdownTree};

#[tauri::command]
pub fn translate(path: String, app: AppHandle) -> Result<String, Error> {
    LanguageController(app).translate(&path)
}

#[tauri::command]
pub fn help_text(app: AppHandle) -> Result<MarkdownTree, Error> {
    LanguageController(app).help_text()
}

#[tauri::command]
pub fn list_languages(app: AppHandle) -> Result<Vec<(String, String)>, Error> {
    LanguageController(app).list()
}
