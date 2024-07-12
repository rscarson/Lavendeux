use std::{
    fs::{self, File},
    io::Write,
    path::Path,
};

use tauri::{AppHandle, Manager, State};
use tauri_plugin_autostart::ManagerExt;

use crate::{
    bugcheck, debug,
    managed_value::ManagedValue,
    models::{settings::Settings, shortcut::Shortcut},
    FsUtils,
};

use super::{Controller, DebugableResult, LanguageController, ShortcutController, TrayController};

pub struct SettingsController(pub AppHandle);
impl Controller<Settings> for SettingsController {
    const EVENT_NAME: &'static str = "updated-settings";

    fn new_managed() -> Result<crate::ManagedValue<Settings>, String> {
        Ok(ManagedValue::new(Settings::default()))
    }

    fn state(&self) -> State<ManagedValue<Settings>> {
        self.0.state::<ManagedValue<Settings>>()
    }

    fn read(&self) -> Option<Settings> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, Settings>> {
        self.state().inner().read()
    }

    fn write(&self, value: &Settings) -> Result<Settings, String> {
        let last_known_good = self.read().ok_or("could not lock settings")?;
        let mut value = value.clone();
        value.set_root(last_known_good.root_dir().to_owned()); // Do not allow root to be changed here

        // Run handlers
        value = self.update(&value, &last_known_good);

        //
        // All is well - emit new settings
        //

        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        self.save();
        TrayController(self.0.clone())
            .update()
            .debug_ok(&self.0, "update-tray");
        self.emit(&value);

        Ok(value)
    }

    fn emit(&self, value: &Settings) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl SettingsController {
    ///
    /// Apply the current settings to the system
    pub fn apply_self(&self) -> Result<Settings, String> {
        if let Some(settings) = self.read() {
            let settings = self.update(&settings, &settings);
            self.write(&settings)
        } else {
            Err("could not lock settings".to_string())
        }
    }

    ///
    /// Apply a settings object to the system
    pub fn update(&self, new: &Settings, prev: &Settings) -> Settings {
        let mut settings = new.clone();

        if self.update_shortcut(&new.shortcut, &prev.shortcut).is_err() {
            settings.shortcut = prev.shortcut.clone();
        }

        if self
            .update_start_with_os(new.start_with_os, prev.start_with_os)
            .is_err()
        {
            settings.start_with_os = prev.start_with_os;
        }

        if self
            .update_language_code(&new.language_code, &prev.language_code)
            .is_err()
        {
            println!("Could not update language");
            settings.language_code = prev.language_code.clone();
        }

        settings
    }

    ///
    /// Update the registered shortcut
    pub fn update_shortcut(&self, new: &Shortcut, prev: &Shortcut) -> Result<(), String> {
        if prev == new {
            Ok(())
        } else {
            println!("Registering new shortcut: {:?}", new);
            let shortcut_controller = ShortcutController(self.0.clone());
            shortcut_controller
                .register(new)
                .or(Err("invalid shortcut".to_string()))?;
            if shortcut_controller.unregister(prev).is_err() {
                // The old hotkey was somehow invalid? Stop because there may now be multiple registered hotkeys
                bugcheck::fatal(
                    self.0.clone(),
                    "Hotkey registration error",
                    "error registering hotkey; manager is out of sync! Please report this issue; Multiple hotkeys may be registered!",
                )
            }
            Ok(())
        }
    }

    pub fn update_start_with_os(&self, new: bool, prev: bool) -> Result<(), String> {
        if prev == new {
            Ok(())
        } else if new {
            self.0
                .autolaunch()
                .enable()
                .or(Err("could not enable autostart".to_string()))
        } else {
            self.0
                .autolaunch()
                .disable()
                .or(Err("could not disable autostart".to_string()))
        }
    }

    pub fn update_language_code(&self, new: &str, prev: &str) -> Result<(), String> {
        if prev == new {
            Ok(())
        } else {
            println!("Registering language: {:?}", new);
            let language_controller = LanguageController(self.0.clone());
            language_controller.set_language(new)
        }
    }

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
        debug!(self.0.clone(), "save-settings");
        if let Some(settings) = self.read() {
            let s = serde_json::to_string_pretty(&settings).expect("could not serialize settings");
            if let Ok(mut file) = File::create(settings.filename()) {
                file.write_all(s.as_bytes())
                    .debug_ok(&self.0, "write-settings-error");
            } else {
                debug!(self.0.clone(), "could not write settings");
            }
        } else {
            debug!(self.0.clone(), "could not lock settings");
        }
    }

    ///
    /// Attempt to load settings from a file
    pub fn load(&self) {
        if let Some(config) = self.read() {
            if let Ok(contents) = std::fs::read_to_string(config.filename()) {
                if let Ok(settings) = serde_json::from_str::<Settings>(&contents) {
                    self.write(&settings).debug_ok(&self.0, "load-settings");
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

    pub fn set_root(&self, root_dir: &Path) {
        self.state()
            .mutate(|settings| {
                settings.set_root(root_dir.to_owned());
                self.emit(settings);
                self.save();
            })
            .debug_ok(&self.0, "set-root");
    }

    pub fn root(&self) -> Option<std::path::PathBuf> {
        self.read().map(|settings| settings.root_dir().to_owned())
    }
}
