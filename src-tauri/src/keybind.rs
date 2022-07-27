use std::{thread::sleep, time::Duration};
use tauri::{AppHandle, GlobalShortcutManager, Manager};
use crate::state::SharedState;
use std::{thread};

#[cfg(target_os="windows")]
mod clipboard_keys {
    use inputbot::KeybdKey;
    pub const MODIFIER : KeybdKey = KeybdKey::LControlKey;
    pub const COPY : KeybdKey = KeybdKey::CKey;
    pub const PASTE : KeybdKey = KeybdKey::VKey;
}

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
mod clipboard_keys {
    use inputbot::KeybdKey;
    pub const MODIFIER : KeybdKey = KeybdKey::LControlKey;
    pub const COPY : KeybdKey = KeybdKey::CKey;
    pub const PASTE : KeybdKey = KeybdKey::VKey;
}

#[cfg(target_os="macos")]
mod clipboard_keys {
    use inputbot::KeybdKey;
    pub const MODIFIER : KeybdKey = KeybdKey::LControlKey;
    pub const COPY : KeybdKey = KeybdKey::CKey;
    pub const PASTE : KeybdKey = KeybdKey::VKey;
}

fn handle_shortcut(app_handle: AppHandle, handler: fn(AppHandle)) {
    thread::spawn(move || {
        let state: tauri::State<SharedState> = app_handle.state();
        if let Ok(mut lock) = state.0.lock() {
            lock.logger.debug(&app_handle, "Handling keyboard shortcut!");
        }
        
        handler(app_handle.clone());
    });
}

/// Bind a new shortcut to the host
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `shortcut` - New shortcut
/// * `default_shortcut` - Fallback shortcut
/// * `handler` - Handler function
pub fn bind_shortcut(app_handle: tauri::AppHandle, shortcut: &str, default_shortcut: &str, handler: fn(AppHandle)) -> Option<String> {
	let mut gsm = app_handle.global_shortcut_manager();
	gsm.unregister_all().ok()?;
	let mut _app_handle = app_handle.clone();
	match gsm.register(shortcut, move || handle_shortcut(_app_handle.clone(), handler)) {
		Ok(_) => None,
		Err(_) => {
			// Error - put default shortcut back in
	        _app_handle = app_handle.clone();
			gsm.register(default_shortcut, move || handle_shortcut(_app_handle.clone(), handler)).ok()?;
			Some("invalid shortcut".to_string())
		}
	}
}

/// Send a copy-command to the host
pub fn send_copy() {
    clipboard_keys::MODIFIER.release(); clipboard_keys::COPY.release();
    sleep(Duration::from_millis(50));
    clipboard_keys::MODIFIER.press(); clipboard_keys::COPY.press();
    sleep(Duration::from_millis(50));
    clipboard_keys::MODIFIER.release(); clipboard_keys::COPY.release();
}

/// Send a paste-command to the host
pub fn send_paste() {
    clipboard_keys::MODIFIER.release(); clipboard_keys::COPY.release();
    sleep(Duration::from_millis(50));
    clipboard_keys::MODIFIER.press(); clipboard_keys::PASTE.press();
    sleep(Duration::from_millis(50));
    clipboard_keys::MODIFIER.release(); clipboard_keys::PASTE.release();
}
