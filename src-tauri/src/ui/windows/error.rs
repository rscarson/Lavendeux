use std::{thread, time};
use tauri::{
    AppHandle, Window, Manager, Position, 
    PhysicalPosition
};

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
pub struct Error(Window, AppHandle);
impl Error {
    /// Create a new handle to the error window
    /// 
    /// # Arguments
    /// * `app_handle` - AppHandle
    pub fn new(app_handle: AppHandle) -> Option<Error> {
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

    /// Hide the window menu bar
    pub fn hide_menu(&self) {
        self.0.menu_handle().hide().ok();
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
            x: ERROR_WINDOW_X_OFFSET as i32,
            y: ERROR_WINDOW_Y_OFFSET as i32
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