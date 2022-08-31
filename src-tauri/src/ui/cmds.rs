use tauri::{AppHandle, Manager};
use crate::core::SharedState;

use crate::core::settings;
use crate::ui::{ app::App };

use crate::utils::language::Language;
use crate::utils::logs;

use crate::ui::tray::Tray;
use crate::core::history;

use lavendeux_parser::Extension;
use crate::core::extensions;

////////////////////////////////////////////////
/// Settings commands
/// 
////////////////////////////////////////////////

/// Format the configured shortcut as a human-readable string
/// 
/// # Arguments
/// * `state` - Application state
#[tauri::command]
pub fn format_shortcut(state: tauri::State<SharedState>) -> Result<String, String> {
	match state.0.lock().ok() {
		Some(lock) => {
			Ok(settings::shortcut_name(&lock.settings.shortcut))
		},

		None => Err("Could not lock settings object".to_string())
	}
}

/// Return current application settings
/// 
/// # Arguments
/// * `_app_handle` - AppHandle
/// * `state` - Application state
#[tauri::command]
pub fn get_settings(_app_handle: AppHandle, state: tauri::State<SharedState>) -> Option<settings::Settings> {
	match state.0.lock().ok() {
		Some(lock) => {
			Some(lock.settings.clone())
		},
		None => None
	}
}

/// Update the current application settings
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Application state
/// * `settings` - Application settings
#[tauri::command]
pub fn update_settings(app_handle: AppHandle, state: tauri::State<SharedState>, settings: settings::Settings) -> Result<settings::Settings, String> {
	match state.0.try_lock() {
		Ok(mut lock) => {
			App::update_settings(&app_handle, &mut lock, settings)
		},

		Err(e) => {
			println!("Could not lock state: {}", e);
			Err("Could not lock settings object".to_string())
		}
	}
}

////////////////////////////////////////////////
/// Language commands
/// 
////////////////////////////////////////////////
 
/// Get language strings for a given language
/// 
/// # Arguments
/// * `app_handle` - Application handle
/// * `state` - Application state
#[tauri::command]
pub fn get_lang(state: tauri::State<SharedState>) -> Result<Language, String> {
	match state.0.lock().ok() {
		Some(lock) => {
			Ok(Language::get(&lock.settings.language))
		},

		None => Err("Could not lock settings object".to_string())
	}
}

////////////////////////////////////////////////
/// Logger commands
/// 
////////////////////////////////////////////////

/// Get the latest log entries
/// 
/// # Arguments
/// * `app_handle` - Application handle
/// * `state` - Application state
#[tauri::command]
pub fn get_logs(state: tauri::State<SharedState>) -> Result<Vec<logs::Entry>, String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			Ok(logs::get_logs(&mut lock))
		},

		None => Err("Could not lock settings object".to_string())
	}
}

/// Open the log directory in the system explorer
/// 
/// # Arguments
/// * `app_handle` - Application handle
/// * `state` - Current application state
#[tauri::command]
pub fn clear_logs(app_handle: AppHandle, state: tauri::State<SharedState>) {
    if let Ok(mut lock) = state.0.lock() {
        logs::clear_logs(&mut lock);
        app_handle.emit_all("logs", logs::get_logs(&mut lock)).unwrap();
    }
}

/// Open the log directory in the system explorer
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn open_logs_dir(state: tauri::State<SharedState>) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			logs::open_logs_dir(&mut lock);
            Ok(())
		},
		None => Err("Could not lock settings object".to_string())
	}
}

////////////////////////////////////////////////
/// History commands
/// 
////////////////////////////////////////////////

/// Clear current history
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn clear_history(app_handle: AppHandle, state: tauri::State<SharedState>) -> Option<String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
            history::clear(&mut lock);
            
            // Update tray history
            let recent_history = history::recent(&mut lock);
            let tray = Tray::new(app_handle.tray_handle());
            tray.update_menu(recent_history);
            
	        app_handle.emit_all("history", lock.history.clone()).ok()?;
            None
		},
		None => Some("Could not lock settings object".to_string())
	}

}

////////////////////////////////////////////////
/// Extension commands
/// 
////////////////////////////////////////////////

/// Load an extension
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn import_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			extensions::import(&mut lock, src_path)?;
            app_handle.emit_all("extensions", extensions::all(&mut lock)).ok();
            Ok(())
		},
		None => Err("Could not lock settings object".to_string())
	}
}

/// Disable an extension
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn disable_extension(app_handle: AppHandle, state: tauri::State<SharedState>, src_path: &str) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			extensions::disable(&mut lock, src_path)?;
            app_handle.emit_all("extensions", extensions::all(&mut lock)).ok();
            Ok(())
		},
		None => Err("Could not lock settings object".to_string())
	}
}

/// Get all extensions
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn get_extensions(state: tauri::State<SharedState>) -> Result<Vec<Extension>, String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			extensions::all(&mut lock)
		},
		None => Err("Could not lock settings object".to_string())
	}
}

/// Open the extension directory in the system explorer
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn open_extensions_dir(state: tauri::State<SharedState>) -> Result<(), String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			extensions::open_extensions_dir(&mut lock);
            Ok(())
		},
		None => Err("Could not lock settings object".to_string())
	}
}

/// Reload extensions
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Current application state
#[tauri::command]
pub fn reload_extensions(app_handle: AppHandle, state: tauri::State<SharedState>) -> Result<Vec<Extension>, String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
            let extensions = extensions::reload(&mut lock);
            app_handle.emit_all("extensions", extensions.clone()).ok();
			extensions
		},
		None => Err("Could not lock settings object".to_string())
	}
}