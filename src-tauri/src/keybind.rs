use std::{thread::sleep, time::Duration};
use tauri::{AppHandle, GlobalShortcutManager};
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
	match gsm.register(shortcut, move || {
		let app_handle = _app_handle.clone();
        
        thread::spawn(move || {
            handler(app_handle);
        });
		
	}) {
		Ok(_) => None,
		Err(e) => {
			// Error - put default shortcut back in
			gsm.register(default_shortcut, move || {
				let app_handle = app_handle.clone();
				handler(app_handle);
			}).ok()?;
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