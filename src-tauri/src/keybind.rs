use std::{thread::sleep, time::Duration};
use tauri::{AppHandle, GlobalShortcutManager};
use inputbot::KeybdKey;
use std::{thread};

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
		Err(_) => {
			// Error - put default shortcut back in
			_app_handle = app_handle.clone();
			gsm.register(default_shortcut, move || {
				let app_handle = _app_handle.clone();
				handler(app_handle);
			}).ok()?;
			Some("invalid shortcut".to_string())
		}
	}
}

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
pub fn send_copy() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::CKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
}

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
pub fn send_paste() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::VKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::VKey.release();
}

#[cfg(target_os="windows")]
pub fn send_copy() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::CKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
}

#[cfg(target_os="windows")]
pub fn send_paste() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::VKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::VKey.release();
}

#[cfg(target_os="macos")]
pub fn send_copy() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::CKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
}

#[cfg(target_os="macos")]
pub fn send_paste() {
    KeybdKey::LControlKey.release(); KeybdKey::CKey.release();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.press(); KeybdKey::VKey.press();
    sleep(Duration::from_millis(50));
    KeybdKey::LControlKey.release(); KeybdKey::VKey.release();
}