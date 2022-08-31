use serde::{Deserialize, Serialize};
use std::path::Path;
use dirs::home_dir;

use crate::utils::{ autostart, language, fs };
use crate::core::{ State, extensions };

pub const DEFAULT_SHORTCUT : &str = "CmdOrCtrl+Space";
pub const DEFAULT_ROOTDIR : &str = ".lavendeux";

#[cfg(any(target_os="windows", target_os="macos"))]
pub const DEFAULT_AUTOPASTE : bool = true;

#[cfg(all(unix, not(any(target_os="macos", target_os="android", target_os="emscripten"))))]
pub const DEFAULT_AUTOPASTE : bool = false;

/// Application settings
#[derive(Serialize, Deserialize, Clone)]
pub struct Settings {
	#[serde(default)]
	pub filename: String,
	
	#[serde(default)]
	pub logname: String,

	pub auto_paste: bool,
	pub silent_errors: bool,
	pub extension_dir: String,
	pub language: String,
	pub shortcut: String,

	pub autostart: bool,
	pub dark: bool
}

impl Default for Settings {
	fn default() -> Self {
		Self {
			filename: "lavendeux.config.json".to_string(),
			logname: "lavendeux.log".to_string(),

			auto_paste: DEFAULT_AUTOPASTE,
			silent_errors: false,
			extension_dir: "extensions".to_string(),
			language: language::DEFAULT.to_string(),
			shortcut: DEFAULT_SHORTCUT.to_string(),
			autostart: false,
			dark: false
		}
	}
}

impl Settings {
	/// Initialise blank settings
	pub fn new() -> Result<Self, String> {
		let mut settings: Self = Self::default();
		let home = home_dir().ok_or("Unable to get user's home directory".to_string())?;

		settings.filename = fs::merge_paths(&home, &[
			DEFAULT_ROOTDIR.to_string(), settings.filename
		])?;

		settings.logname = fs::merge_paths(&home, &[
			DEFAULT_ROOTDIR.to_string(), settings.logname
		])?;

		settings.extension_dir = fs::merge_paths(&home, &[
			DEFAULT_ROOTDIR.to_string(), settings.extension_dir
		])?;
		
		Ok(settings)
	}

	/// Create a new settings file, if none exists
	pub fn create() -> Result<Self, String> {
		let settings = Self::new()?;
		if let Ok(result) = Self::read(&settings.filename) {
			Ok(result)
		} else {
			Ok(settings)
		}
	}

	/// Read settings from a file
	/// 
	/// # Arguments
	/// * `path` - Path to the config file
	pub fn read(path: &str) -> Result<Self, String> {
		match std::fs::read_to_string(path) {
			Ok(json) => {
				let settings : Self = serde_json::from_str(&json).or(Err(format!("{} was not valid JSON", path)))?;
				Ok(settings)
			},
			Err(e) => {
				Err(e.to_string())
			}
		}
	}

	/// Write settings to a file
	pub fn write(&self) -> Result<(), String> {
		let json = serde_json::to_string(self).or(Err(format!("Unknown error serializing settings")))?;
		if let Err(e) = std::fs::write(self.filename.clone(), json) {
			Err(format!("Error writing to settings: {}", e))
		} else {
			Ok(())
		}
	}

	// Verify settings and propagate changes
	/// 
	/// # Arguments
	/// * `state` - Current application state
	pub fn update(&self, state: &mut State) -> Result<(), String> {
		state.logger.debug("Updating settings");
			
		// Create the extensions dir if needed
		if let Err(e) = std::fs::create_dir_all(
			Path::new(&(fs::compile_path(&[ self.extension_dir.clone(), "disabled_extensions".to_string() ])?))
		) {
			let err = format!("Error creating the .lavendeux/extensions dir: {}", e);
			state.logger.error(&err);
			return Err(err);
		}
		extensions::reload(state)?;

		// Update autostart
		if let Some(e) = autostart::update(self.autostart) {
			let err = format!("Error updating autostart: {}", e);
			state.logger.error(&err);
			return Err(err);
		}

		// Update state
		state.settings = self.clone();

		// Save settings to file
		if let Err(e) = self.write() {
			let err = format!("Error writing settings to '{}': {}", self.filename.clone(), e);
			state.logger.error(&err);
			return Err(err);
		}

		state.logger.debug("Succesfully updated settings");
		Ok(())
	}
}

/// Get a friendly representation of the current shortcut
/// 
/// # Arguments
/// * `settings` - Application settings
pub fn shortcut_name(shortcut: &str) -> String {
	if cfg!(target_os = "macos") {
		shortcut.replace("CmdOrCtrl", "Cmd")
	} else {
		shortcut.replace("CmdOrCtrl", "Ctrl")
	}
}