use std::{thread, time};
use crate::history;
use tauri::{
    AppHandle, Window, Manager, Position, WindowMenuEvent, 
    Menu, Submenu, CustomMenuItem, MenuItem, PhysicalPosition
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
        let w = self.0.clone();
        thread::spawn(move || {
            w.show().ok();
        });
        self.0.set_always_on_top(true)?;
        self.0.request_user_attention(Some(tauri::UserAttentionType::Informational))?;
        self.0.set_always_on_top(false)?;
    
        // Set tab
        self.0.emit_all("switch_tab", tab)?;
        Ok(())
    }

    /// Show a dialog to the user
    /// 
    /// # Arguments
    /// * `title` - Window title
    /// * `msg` - Message contents
    pub fn show_dialog(&self, title: &str, message: &str) {
        tauri::api::dialog::message(Some(&self.0), title, message);
    }

    /// Hide the main window
    pub fn hide(&self) -> tauri::Result<()> {
        let w = self.0.clone();
        thread::spawn(move || {
            w.hide().ok();
        });
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
                let app = e.window().app_handle();
                let config = app.config();

                tauri::api::dialog::message(Some(e.window()), "About Lavendeux", 
                    format!("{} version {}\nParse anywhere.\nDevelopped by @rscarson", 
                        config.package.product_name.as_ref().unwrap(), config.package.version.as_ref().unwrap()
                    )
                );
            },

            "exit" => {
                let app = e.window().app_handle();
                std::thread::spawn(move || {
                    app.exit(0);
                });
            },

            _ => {}
        }
    }
}

const ERROR_WINDOW_X_OFFSET : u32 = 3;
const ERROR_WINDOW_Y_OFFSET : u32 = 32;
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

    /// Hide the error window
    pub fn hide(&self) -> tauri::Result<()> {
        let w = self.0.clone();
        thread::spawn(move || {
            w.hide().ok();
        });
        Ok(())
    }

    /// Recalculate error window position 
    #[cfg(any(target_os="windows", target_os="macos"))]
    pub fn calculate_position(&self) -> tauri::Result<PhysicalPosition<i32>> {
        let monitor = self.0.current_monitor()?.ok_or_else(|| tauri::Error::AssetNotFound("unable to find current monitor".to_string()))?;
        let window_size = self.0.outer_size()?;
    
        // Calculate position for the error window
        let mut pos = self.0.outer_position()?;
        pos.x = (monitor.size().width - window_size.width - ERROR_WINDOW_X_OFFSET) as i32;
        pos.y = (monitor.size().height - window_size.height - ERROR_WINDOW_Y_OFFSET) as i32;

        Ok(pos)
    }

    /// Recalculate error window position 
    #[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
    pub fn calculate_position(&self) -> tauri::Result<PhysicalPosition<i32>> {
        Ok(PhysicalPosition {
            x: ERROR_WINDOW_OFFSET as i32,
            y: ERROR_WINDOW_OFFSET as i32
        })
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
        let pos = self.calculate_position()?;
    
        // Set window position and get focus
        app_handle.emit_all("message", MessagePayload {
            title: title.to_string(),
            msg: msg.to_string(), 
            variant: payload.to_string()
        })?;
        
        let w = self.0.clone();
        thread::spawn(move || {
            w.show().ok();
            w.set_position(Position::Physical(pos)).ok();
            w.set_always_on_top(true).ok();
        
            // Hide the window after a delay
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
    ErrorWindow::new(app_handle).unwrap().hide().ok();
}

/// Show the history tab
/// 
/// # Arguments
/// * `app_handle` - AppHandle
#[tauri::command]
pub fn show_history_tab(app_handle: AppHandle) {
    MainWindow::new(app_handle).unwrap().show_tab(WindowTabs::History).ok();
}