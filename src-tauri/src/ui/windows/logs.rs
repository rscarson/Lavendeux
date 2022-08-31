use crate::utils::language::Language;
use tauri::{ Window };

/// Log view window
pub struct Logs(Window);
impl Logs {
    /// Create and display a new log window
    /// 
    /// # Arguments
    /// * `app_handle` - AppHandle
    pub fn new(parent: &Window) -> Option<Logs> {
        let lang = Language::get("en");
        let w = Window::builder(
            parent,
            "logs".to_string(),
            tauri::WindowUrl::App("/logs".into())
        )
            .title(lang.logviewer_title)
            .menu(tauri::Menu::new())
            .build().ok()?;
        Some(Self(w))
    }
}