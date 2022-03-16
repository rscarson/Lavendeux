use crate::{parser, keybind, extensions};
use crate::state::SharedState;
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager};
use std::path::{PathBuf};
use dirs::home_dir;

const DEFAULT_SHORTCUT : &str = "CmdOrCtrl+Space";
const DEFAULT_EXTDIR : &str = ".lavendeux";

/// Application settings
#[derive(Serialize, Deserialize, Clone)]
pub struct Settings {
	pub auto_paste: bool,
	pub silent_errors: bool,
	pub extension_dir: String,
	pub shortcut: String
}

/// Get a friendly representation of the current shortcut
/// 
/// # Arguments
/// * `settings` - Application settings
pub fn shortcut_name(settings: &Settings) -> String {
	if cfg!(target_os = "macos") {
		settings.shortcut.replace("CmdOrCtrl", "Cmd")
	} else {
		settings.shortcut.replace("CmdOrCtrl", "Ctrl")
	}
}

impl Settings {
	/// Initialise blank settings
	pub fn new() -> Self {
		let mut ext_path = home_dir().unwrap_or(PathBuf::new());
		ext_path.push(DEFAULT_EXTDIR);

		Self {
			shortcut: DEFAULT_SHORTCUT.to_string(),
			extension_dir: ext_path.to_str().unwrap_or(DEFAULT_EXTDIR).to_string(),
			silent_errors: false,
			auto_paste: true
		}
	}
}

/// Update the current application settings
/// 
/// # Arguments
/// * `app_handle` - AppHandle
/// * `state` - Application state
/// * `settings` - Application settings
#[tauri::command]
pub fn update_settings(app_handle: AppHandle, state: tauri::State<SharedState>, settings: Settings) -> Result<Settings, String> {
	match state.0.lock().ok() {
		Some(mut lock) => {
			// Update keyboard shortcut
			match keybind::bind_shortcut(app_handle.clone(), &settings.shortcut, DEFAULT_SHORTCUT, parser::handle_shortcut) {
				Some(e) => return Err(e),
				None => {}
			}
			
			// Create the extensions dir, if needed and load them
			let mut path = PathBuf::new();
			path.push(&settings.extension_dir);
			match std::fs::create_dir_all(path.to_str().ok_or("unicode error")?) {
				Err(e) => return Err(e.to_string()),
				_ => {}
			}

			// Create a subdirectory
			path.push("disabled_extensions");
			match std::fs::create_dir_all(path.to_str().ok_or("unicode error")?) {
				Err(e) => return Err(e.to_string()),
				_ => {}
			}

			// Reload extensions
			extensions::reload_extensions(app_handle.clone(), &mut lock)?;
			
			// Lock in settings
			lock.settings = settings.clone();
			app_handle.emit_all("settings", settings.clone()).unwrap();
			Ok(settings)
		},

		None => Err("Could not lock settings object".to_string())
	}
}