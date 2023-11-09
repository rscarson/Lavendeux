use tauri::AppHandle;

use crate::{controllers::LanguageController, models::markdown::MarkdownTree};

#[tauri::command]
pub fn translate(path: String, app: AppHandle) -> Option<String> {
    LanguageController(app).translate(&path)
}

#[tauri::command]
pub fn help_text(app: AppHandle) -> Option<MarkdownTree> {
    LanguageController(app).help_text()
}

#[tauri::command]
pub fn list_languages(app: AppHandle) -> Vec<(String, String)> {
    LanguageController(app).list()
}
