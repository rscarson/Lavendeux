use serde::{Deserialize, Serialize};

use crate::utils::{ fs };
use crate::core::language;

pub const DEFAULT_CONFIG_FILENAME : &str = "lavendeux.config.json";
pub const DEFAULT_LOG_FILENAME : &str = "lavendeux.log";
pub const DEFAULT_STARTUP_FILENAME : &str = "onstart.lav";
pub const DEFAULT_EXTENSIONS_DIR : &str = "extensions";

pub const DEFAULT_SHORTCUT : &str = "CmdOrCtrl+Space";
pub const DEFAULT_ROOTDIR : &str = ".lavendeux";

pub const DEFAULT_AUTOPASTE : bool = true;

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct WriteableSettings {
	pub logname: String,
	pub script: String,

	pub auto_paste: bool,
	pub autostart: bool,
	pub dark: bool,
	pub silent_errors: bool,

	pub extension_dir: String,
	pub language: String,
	pub shortcut: String,
}

impl Default for WriteableSettings {
	fn default() -> Self {
		Self {
			logname: DEFAULT_LOG_FILENAME.to_string(),
			script: DEFAULT_STARTUP_FILENAME.to_string(),

			auto_paste: DEFAULT_AUTOPASTE,
			autostart: false,
			dark: false,
			silent_errors: false,

			extension_dir: DEFAULT_EXTENSIONS_DIR.to_string(),
			language: language::DEFAULT.to_string(),
			shortcut: DEFAULT_SHORTCUT.to_string(),
		}
	}
}

impl WriteableSettings {
	pub fn fix_paths(&mut self) -> Option<String> {
		self.logname = fs::fix_relative_path(&[DEFAULT_ROOTDIR.to_string(), self.logname.clone()]).ok()?;
		self.extension_dir = fs::fix_relative_path(&[DEFAULT_ROOTDIR.to_string(), self.extension_dir.clone()]).ok()?;
		self.script = fs::fix_relative_path(&[DEFAULT_ROOTDIR.to_string(), self.script.clone()]).ok()?;
		
		None
	}
}


#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct Settings {
	#[serde(flatten)]
	pub inner_settings: WriteableSettings,
	
	pub filename: String,
	pub onstart: String,
}

impl Default for Settings {
	fn default() -> Self {
		Self {
			filename: DEFAULT_CONFIG_FILENAME.to_string(),
			onstart: String::default(),
			inner_settings: WriteableSettings::default()
		}
	}
}

impl Settings {
	/// Initialise blank settings
	pub fn new() -> Result<Self, String> {
		let mut settings: Self = Self::default();
		if let Some(e) = settings.fix_paths() {
			Err(e)
		} else {
			Ok(settings)
		}
	}

	pub fn fix_paths(&mut self) -> Option<String> {
		self.filename = fs::fix_relative_path(&[DEFAULT_ROOTDIR.to_string(), self.filename.clone()]).ok()?;
		self.inner_settings.fix_paths();
		
		None
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
				let mut settings : Self = Self::default();
				settings.fix_paths();
				settings.inner_settings = serde_json::from_str(&json).or(Err(format!("{} was not valid JSON", path)))?;

				match std::fs::read_to_string(settings.inner_settings.script.clone()) {
					Ok(script) => {
						settings.onstart = script;
						Ok(settings)
					},
					Err(e) => {
						Err(format!("Error reading from {}: {}", settings.inner_settings.script.clone(), e))
					}
				}
			},
			Err(e) => {
				Err(e.to_string())
			}
		}
	}

	/// Write settings to a file
	pub fn write(&self) -> Result<(), String> {
		let json = serde_json::to_string(&self.inner_settings).or(Err(format!("Unknown error serializing settings")))?;

		if let Err(e) = std::fs::write(self.filename.clone(), json) {
			return Err(format!("Error writing to {}: {}", self.filename.clone(), e));
		}

		if let Err(e) = std::fs::write(self.inner_settings.script.clone(), self.onstart.clone()) {
			return Err(format!("Error writing to {}: {}", self.inner_settings.script.clone(), e));
		}

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