use inputbot::KeybdKey;
use std::{thread, time::Duration};
use tauri::{ Manager, AppHandle, GlobalShortcutManager };

use crate::core::{ SharedState, State };

type HotkeyHandler = fn(&AppHandle, &mut State);

/// Send a single keypress to the system
/// 
/// # Arguments
/// * `key` - Key to send
pub fn send_key(key: KeybdKey, release: bool) {
    key.release();
    thread::sleep(Duration::from_millis(20));
    key.press();
    if release {
        thread::sleep(Duration::from_millis(20));
        key.release();
    }
}

/// Send a sequence of keypresses to the system
/// 
/// # Arguments
/// * `sequence` - Keys to send
pub fn send_sequence(mod_key: KeybdKey, sequence: &[KeybdKey]) {
    send_key(mod_key, false);
    for key in sequence { send_key(*key, true) }
    mod_key.release();
}

/// Send a CTRL-C key sequence to the system
pub fn send_copy() {
    send_sequence(KeybdKey::LControlKey, &[
        KeybdKey::CKey
    ]);
}

/// Send a CTRL-V key sequence to the system
pub fn send_paste() {
    send_sequence(KeybdKey::LControlKey, &[
        KeybdKey::VKey
    ]);
}

/// Spawns a thread to handle the hotkey
fn spawn_handler(app_handle: AppHandle, handler: HotkeyHandler) {
    thread::spawn(move || {
        let state: tauri::State<SharedState> = app_handle.state();
        if let Ok(mut lock) = state.0.lock() {
            lock.logger.debug("Handling keyboard event");
            handler(&app_handle, &mut lock);
        };
    });
}

/// Bind a hotkey sequence to a handler
/// 
/// # Arguments
/// * `app_handle` - App handle
/// * `state` - Application state
/// * `hotkey` - Key sequence to watch for
/// * `handler` - Handler to run
pub fn bind_shortcut(app_handle: &AppHandle, state: &mut State, hotkey: &str, handler: HotkeyHandler) -> Option<String> {
    let _app_handle = app_handle.clone();
    let __app_handle = app_handle.clone();
	let mut manager = app_handle.global_shortcut_manager();
    manager.unregister_all().ok()?;
	match manager.register(hotkey, move || spawn_handler(_app_handle.clone(), handler)) {
		Ok(_) => {
            state.logger.debug(&format!("Registered shortcut {}", hotkey));
            None
        },
		Err(e) => {
			// Error - put last working shortcut back in
            let error = format!("Could not register shortcut {}: {}", hotkey, e);
            state.logger.warning(&error);
			manager.register(&state.settings.shortcut, move || spawn_handler(__app_handle.clone(), handler)).ok()?;
			Some(error)
		}
	}
}