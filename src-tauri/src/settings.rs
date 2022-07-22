use crate::{parser, keybind, extensions, autostart};
use crate::state::SharedState;
use crate::windows::ErrorWindow;
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager};
use std::path::PathBuf;
use std::fs::File;
use std::io::{BufReader, BufWriter};
use std::error::Error;
use dirs::home_dir;

const DEFAULT_SHORTCUT : &str = "CmdOrCtrl+Space";
const DEFAULT_CONFIGNAME : &str = "lavendeux.config.json";
const DEFAULT_LOGNAME : &str = "lavendeux.log";
const DEFAULT_ROOTDIR : &str = ".lavendeux";
const DEFAULT_EXTDIR : &str = "extensions";
const DEFAULT_SILENTERRORS : bool = false;
const DEFAULT_DARK: bool = false;

#[cfg(any(target_os="windows", target_os="macos"))]
const DEFAULT_AUTOPASTE : bool = true;

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
const DEFAULT_AUTOPASTE : bool = false;

/// Application settings
#[derive(Serialize, Deserialize, Clone)]
pub struct Settings {
	#[serde(default)]
	pub filename: String,
	pub logname: String,

	pub auto_paste: bool,
	pub silent_errors: bool,
	pub extension_dir: String,
	pub shortcut: String,

	pub autostart: bool,
	pub dark: bool
}

impl Default for Settings {
	fn default() -> Self {
		Self::new()
	}
}

/// Read settings from a file
/// 
/// # Arguments
/// * `path` - Path to the config file
pub fn read_settings(path : Option<&str>) -> Result<Settings, Box<dyn Error>> {
	let filename : String = match path {
		Some(s) => s.to_string(),
		None => {
			let mut root = home_dir().unwrap_or_default();
			root.push(DEFAULT_ROOTDIR);
			root.push(DEFAULT_CONFIGNAME);
			root.to_str().unwrap_or(DEFAULT_CONFIGNAME).to_string()
		}
	};

	let file = File::open(filename.clone())?;
	let reader = BufReader::new(file);
	let mut settings : Settings = serde_json::from_reader(reader)?;
	settings.filename = filename;

	Ok(settings)
}

/// Write settings to a file
/// 
/// # Arguments
/// * `settings` - Application settings
pub fn write_settings(settings: &Settings) -> Result<(), Box<dyn Error>> {
	let file = File::create(settings.filename.clone())?;
	let writer = BufWriter::new(file);
	serde_json::to_writer(writer, settings)?;
	Ok(())
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
		match read_settings(None) {
			Ok(s) => s,
			Err(_) => {
				let mut path = home_dir().unwrap_or_default();
				path.push(DEFAULT_ROOTDIR);
				path.push(DEFAULT_EXTDIR);
				let ext_dir = path.to_str().unwrap_or(DEFAULT_ROOTDIR).to_string();

				path.pop();
				path.push(DEFAULT_CONFIGNAME);
				let config_path = path.to_str().unwrap_or(DEFAULT_CONFIGNAME).to_string();

				path.pop();
				path.push(DEFAULT_LOGNAME);
				let log_path = path.to_str().unwrap_or(DEFAULT_LOGNAME).to_string();

				Self {
					filename: config_path,
					logname: log_path,

					shortcut: DEFAULT_SHORTCUT.to_string(),
					extension_dir: ext_dir,
					silent_errors: DEFAULT_SILENTERRORS,
					auto_paste: DEFAULT_AUTOPASTE,
					autostart: false,
					dark: DEFAULT_DARK
				}
			}
		}
	}
}

/// Format the configured shortcut as a human-readable string
/// 
/// # Arguments
/// * `state` - Application state
#[tauri::command]
pub fn format_shortcut(state: tauri::State<SharedState>) -> Result<String, String> {
	match state.0.lock().ok() {
		Some(lock) => {
			Ok(shortcut_name(&lock.settings))
		},

		None => Err("Could not lock settings object".to_string())
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
			lock.logger.debug(&app_handle, "Updating settings");

			// Update keyboard shortcut
			if let Some(e) = keybind::bind_shortcut(app_handle.clone(), &settings.shortcut, DEFAULT_SHORTCUT, parser::handle_shortcut) {
				lock.logger.error(&app_handle, &format!("Error updating the keyboard shortcut: {}", e));
				return Err(e);
			}
			
			// Create the extensions dir, if needed and load them
			let mut path = PathBuf::new();
			path.push(&settings.extension_dir);
			if let Err(e) = std::fs::create_dir_all(path.to_str().ok_or("unicode error")?) {
				lock.logger.error(&app_handle, &format!("Error creating the .lavendeux/extensions dir: {}", e));
				return Err(e.to_string());
			}

			// Create a subdirectory
			path.push("disabled_extensions");
			if let Err(e) = std::fs::create_dir_all(path.to_str().ok_or("unicode error")?) {
				lock.logger.error(&app_handle, &format!("Error creating the .lavendeux/disabled_extensions dir: {}", e));
				return Err(e.to_string());
			}

			// Reload extensions
			extensions::reload_extensions(app_handle.clone(), &mut lock)?;

			// Set autostart
			if settings.autostart {
				if let Some(e) = autostart::set() {
					let error_window = ErrorWindow::new(app_handle.clone()).ok_or("Error window does not exist")?;
					lock.logger.error(&app_handle, &format!("Error setting autostart: {}", e));
					error_window.show_message("Error setting autostart", &e, "danger").ok();
				}
			} else {
				if let Some(e) = autostart::clear() {
					let error_window = ErrorWindow::new(app_handle.clone()).ok_or("Error window does not exist")?;
					lock.logger.error(&app_handle, &format!("Error clearing autostart: {}", e));
					error_window.show_message("Error setting autostart", &e, "danger").ok();
				}
			}
			
			// Lock in settings
			lock.settings = settings.clone();
			app_handle.emit_all("settings", settings.clone()).unwrap();

			if let Err(e) = write_settings(&settings) {
				lock.logger.error(&app_handle, &format!("Error writing settings to '{}': {}", settings.filename.clone(), e));
			}
			Ok(settings)
		},

		None => Err("Could not lock settings object".to_string())
	}
}
