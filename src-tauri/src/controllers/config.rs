use std::{
    fs::{self, File},
    io::Write,
    path::Path,
};

use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Manager, State};

use super::{BlacklistController, Controller, DebugableResult, SettingsController};
use crate::{
    managed_value::ManagedValue,
    models::{config::ConfigurationSettings, extension::Blacklist, settings::Settings},
    FsUtils,
};

#[derive(Serialize, Deserialize)]
struct WriteableSettings {
    config: Settings,
    disabled_extensions: Blacklist,
}

pub struct ConfigController(pub AppHandle);
impl Controller<ConfigurationSettings> for ConfigController {
    const EVENT_NAME: &'static str = "updated-config";

    fn new_managed() -> ManagedValue<ConfigurationSettings> {
        ManagedValue::new(ConfigurationSettings::default())
    }

    fn state(&self) -> State<ManagedValue<ConfigurationSettings>> {
        self.0.state::<ManagedValue<ConfigurationSettings>>()
    }

    fn read(&self) -> Option<ConfigurationSettings> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> std::option::Option<std::sync::MutexGuard<'_, ConfigurationSettings>> {
        self.state().inner().read()
    }

    fn write(&self, value: &ConfigurationSettings) -> Result<ConfigurationSettings, String> {
        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        Ok(value.clone())
    }

    fn emit(&self, value: &ConfigurationSettings) {
        self.0.emit(Self::EVENT_NAME, value.clone()).ok();
    }
}

impl ConfigController {
    ///
    /// Attempt to create the directories needed for the app
    pub fn create_all(&self) -> Result<(), String> {
        if let Some(config) = self.read() {
            fs::create_dir_all(config.ext_dir()).or_else(|e| Err(e.to_string()))
        } else {
            Err("could not lock configuration".to_string())
        }
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_config_dir(&self) {
        if let Some(config) = self.read() {
            FsUtils::open_dir(self.0.clone(), &config.root_dir())
        }
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_ext_dir(&self) {
        if let Some(config) = self.read() {
            FsUtils::open_dir(self.0.clone(), &config.ext_dir())
        }
    }

    ///
    /// Attempt to write settings to a file
    pub fn save(&self) {
        let settings = SettingsController(self.0.clone())
            .read()
            .unwrap_or_default();
        let blacklist = BlacklistController(self.0.clone())
            .read()
            .unwrap_or_default();

        let s = WriteableSettings {
            config: settings,
            disabled_extensions: blacklist,
        };

        let s = serde_json::to_string_pretty(&s).expect("could not serialize settings");
        if let Some(config) = self.read() {
            if let Ok(mut file) = File::create(config.filename()) {
                file.write_all(s.as_bytes())
                    .debug_ok(&self.0, "write-configfile");
            }
        }
    }

    ///
    /// Attempt to load settings from a file
    pub fn load(&self) {
        if let Some(config) = self.read() {
            if let Ok(contents) = fs::read_to_string(config.filename()) {
                if let Ok(json_config) = serde_json::from_str::<WriteableSettings>(&contents) {
                    let settings = json_config.config;
                    let blacklist = json_config.disabled_extensions;

                    SettingsController(self.0.clone())
                        .write(&settings)
                        .debug_ok(&self.0, "load-settings");
                    BlacklistController(self.0.clone())
                        .write(&blacklist)
                        .debug_ok(&self.0, "load-blacklist");
                }
            }
        }
    }

    ///
    /// Attempt to add a new extension
    pub fn add_extension(&self, filename: &Path) -> bool {
        if let Some(config) = self.read() {
            FsUtils::copy(filename, &config.ext_dir())
                .debug_ok(&self.0, "copy-extension")
                .is_some()
        } else {
            false
        }
    }

    ///
    /// Attempt to delete an extension
    pub fn del_extension(&self, filename: &Path) -> bool {
        if let Some(config) = self.read() {
            FsUtils::delete(filename, &config.ext_dir())
                .debug_ok(&self.0, "delete-extension")
                .is_some()
        } else {
            false
        }
    }
}
