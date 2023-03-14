use tauri::{ Window };

use crate::core::SharedState;

/// Log view window
pub struct Logs(Window);
impl Logs {
    /// Create and display a new log window
    /// 
    /// # Arguments
    /// * `app_handle` - AppHandle
    pub fn new(parent: &Window, state: tauri::State<SharedState>) -> Option<Logs> {
        match state.0.lock().ok() {
            Some(lock) => {
                let w = Window::builder(
                    parent,
                    "logs".to_string(),
                    tauri::WindowUrl::App("/logs".into())
                )
                    .title(lock.language["logviewer_title"].to_string())
                    .menu(tauri::Menu::new())
                    .build().ok()?;
                Some(Self(w))
            },
    
            None => None
        }

    }
}