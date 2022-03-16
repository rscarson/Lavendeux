use std::{thread, time};
use crate::history;
use tauri::{
    AppHandle, Window, Manager, Position, WindowMenuEvent, 
    Menu, Submenu, CustomMenuItem, MenuItem
};

/// Tabs on the main window
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub enum WindowTabs {
	History, Extensions, Settings, Help
}

/// Main application window
pub struct MainWindow(Window);
impl MainWindow {
    /// Create a new handle to the main window
    /// 
    /// # Arguments
    /// * `app_handle` - AppHandle
    pub fn new(app_handle: AppHandle) -> Option<MainWindow> {
        Some(Self(app_handle.get_window("main")?))
    }

    /// Show the window, and switch to a tab
    /// 
    /// # Arguments
    /// * `tab` - Tab to switch to
    pub fn show_tab(&self, tab: WindowTabs) -> tauri::Result<()> { 
        // Bring to front
        self.0.show()?;
        self.0.set_always_on_top(true)?;
        self.0.request_user_attention(Some(tauri::UserAttentionType::Informational))?;
        self.0.set_always_on_top(false)?;
    
        // Set tab
        self.0.emit_all("switch_tab", tab)?;
        Ok(())
    }

    /// Create a new menu for the window
    pub fn get_menu() -> Menu {
        Menu::new()
        .add_submenu(Submenu::new("File", Menu::new()
            .add_item(CustomMenuItem::new("clear_history", "Clear History"))
            .add_native_item(MenuItem::Separator)
            .add_item(CustomMenuItem::new("exit", "Exit"))
        ))
        .add_submenu(Submenu::new("Help", Menu::new()
            .add_item(CustomMenuItem::new("about", "About"))
        ))
    }

    /// Handle a menu event
    pub fn handle_menu_event(e: WindowMenuEvent) {
        match e.menu_item_id() {
            "clear_history" => {
                let app = e.window().app_handle();
                history::clear_history(app.clone(), app.state());
            },

            "about" => {
                tauri::api::dialog::message(Some(e.window()), "About Lavendeux", "Parse anywhere.\nDevelopped by @rscarson");
            },

            "exit" => {
                let app = e.window().app_handle();
                let _app = app.clone();
                std::thread::spawn(move || {
                    _app.get_window("main").unwrap().close().unwrap();
                });

                app.exit(0);
                std::process::exit(0);
            },

            _ => {}
        }
    }
}

const ERROR_WINDOW_OFFSET : u32 = 3;
const ERROR_WINDOW_DELAY : u64 = 5000;

/// Payload for a message to the error window
#[derive(Clone, serde::Serialize)]
struct MessagePayload {
	title: String,
	msg: String,
	variant: String
}

/// Get a new handle to the error window
pub struct ErrorWindow(Window, AppHandle);
impl ErrorWindow {
    /// Create a new handle to the error window
    /// 
    /// # Arguments
    /// * `app_handle` - AppHandle
    pub fn new(app_handle: AppHandle) -> Option<ErrorWindow> {
        Some(Self(app_handle.get_window("error")?, app_handle))
    }

    /// Hide the main window
    pub fn hide(&self) -> tauri::Result<()> {
        self.0.hide()
    }

    /// Show a message to the user
    /// 
    /// # Arguments
    /// * `title` - Window title
    /// * `msg` - Message contents
    /// * `payload` - A colour indicator for the message - success, danger, etc
    pub fn show_message(&self, title: &str, msg: &str, payload: &str) -> tauri::Result<()> { 
        // Get relevant handles
        let app_handle = self.1.clone();
        let window_size = self.0.outer_size()?;
        let monitor = self.0.current_monitor()?.ok_or(tauri::Error::AssetNotFound("unable to find current monitor".to_string()))?;
    
        // Calculate position for the error window
        let mut pos = self.0.outer_position()?;
        pos.x = (monitor.size().width - window_size.width - ERROR_WINDOW_OFFSET) as i32;
        pos.y = (monitor.size().height - window_size.height - ERROR_WINDOW_OFFSET) as i32;
    
        // Set window position and get focus
        app_handle.emit_all("message", MessagePayload {
            title: title.to_string(),
            msg: msg.to_string(), 
            variant: payload.to_string()
        })?;
        self.0.show()?;
        self.0.set_position(Position::Physical(pos))?;
        self.0.set_always_on_top(true)?;
    
        // Hide the window after a delay
        let w = self.0.clone();
        thread::spawn(move || {
            thread::sleep(time::Duration::from_millis(ERROR_WINDOW_DELAY));
            w.hide().ok();
        });
    
        Ok(())
    }
}

/// Hide the error window
/// 
/// # Arguments
/// * `app_handle` - AppHandle
#[tauri::command]
pub fn hide_errorwindow(app_handle: AppHandle) {
	match ErrorWindow::new(app_handle.clone()) {
		Some(w) => w.hide().ok(),
		None => Some(())
	};
}