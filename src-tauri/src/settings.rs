use crate::{parser, keybind, extensions};
use crate::state::SharedState;
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager};
use std::path::{PathBuf};
use std::fs::File;
use std::io::{BufReader, BufWriter};
use std::error::Error;
use dirs::home_dir;

const DEFAULT_SHORTCUT : &str = "CmdOrCtrl+Space";
const DEFAULT_CONFIGNAME : &str = "lavendeux.config.json";
const DEFAULT_ROOTDIR : &str = ".lavendeux";
const DEFAULT_EXTDIR : &str = "extensions";

/// Application settings
#[derive(Serialize, Deserialize, Clone)]
pub struct Settings {
	pub auto_paste: bool,
	pub silent_errors: bool,
	pub extension_dir: String,
	pub shortcut: String
}

/// Read settings from a file
/// 
/// # Arguments
/// * `path` - Path to the config file
pub fn read_settings(path : Option<&str>) -> Result<Settings, Box<dyn Error>> {
	let filename : String = match path {
		Some(s) => s.to_string(),
		None => {
			let mut root = home_dir().unwrap_or(PathBuf::new());
			root.push(DEFAULT_ROOTDIR);
			root.push(DEFAULT_CONFIGNAME);
			root.to_str().unwrap_or(DEFAULT_CONFIGNAME).to_string()
		}
	};

	let file = File::open(filename)?;
	let reader = BufReader::new(file);
	Ok(serde_json::from_reader(reader)?)
}

/// Write settings to a file
/// 
/// # Arguments
/// * `settings` - Application settings
/// * `path` - Path to the config file
pub fn write_settings(settings: &Settings, path : Option<&str>) -> Result<(), Box<dyn Error>> {
	let filename : String = match path {
		Some(s) => s.to_string(),
		None => {
			let mut root = home_dir().unwrap_or(PathBuf::new());
			root.push(DEFAULT_ROOTDIR);
			root.push(DEFAULT_CONFIGNAME);
			root.to_str().unwrap_or(DEFAULT_CONFIGNAME).to_string()
		}
	};
	
	let file = File::create(filename)?;
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
				let mut ext_path = home_dir().unwrap_or(PathBuf::new());
				ext_path.push(DEFAULT_ROOTDIR);
				ext_path.push(DEFAULT_EXTDIR);
		
				Self {
					shortcut: DEFAULT_SHORTCUT.to_string(),
					extension_dir: ext_path.to_str().unwrap_or(DEFAULT_ROOTDIR).to_string(),
					silent_errors: false,
					auto_paste: true
				}
			}
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
			write_settings(&settings, None).ok();
			Ok(settings)
		},

		None => Err("Could not lock settings object".to_string())
	}
}