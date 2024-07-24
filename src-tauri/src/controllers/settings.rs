use std::{
    fs::{self, File},
    io::Write,
    path::Path,
};

use tauri::{AppHandle, Manager, State};
use tauri_plugin_autostart::ManagerExt;

use crate::{
    bugcheck, debug,
    error::Error,
    managed_value::ManagedValue,
    models::{settings::Settings, shortcut::Shortcut},
    FsUtils,
};

use super::{Controller, LanguageController, ShortcutController, TrayController};

pub struct SettingsController(pub AppHandle);
impl Controller<Settings> for SettingsController {
    const EVENT_NAME: &'static str = "updated-settings";

    fn new_managed() -> Result<crate::ManagedValue<Settings>, Error> {
        Ok(ManagedValue::new(Settings::default()))
    }

    fn state(&self) -> State<ManagedValue<Settings>> {
        self.0.state::<ManagedValue<Settings>>()
    }

    fn read(&self) -> Result<Settings, Error> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, Settings>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &Settings) -> Result<Settings, Error> {
        let last_known_good = self.read()?;
        let mut value = value.clone();
        value.set_root(last_known_good.root_dir().to_owned()); // Do not allow root to be changed here

        // Run handlers
        value = self.update(&value, &last_known_good);

        //
        // All is well - emit new settings
        //

        self.state().write(value.clone())?;
        self.save()?;
        TrayController(self.0.clone()).update()?;
        self.emit(&value)?;

        Ok(value)
    }

    fn emit(&self, value: &Settings) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, value.clone())?;
        Ok(())
    }
}

impl SettingsController {
    ///
    /// Apply the current settings to the system
    pub fn apply_self(&self) -> Result<Settings, Error> {
        let settings = self.read()?;
        self.update(&settings, &settings);
        self.write(&settings)
    }

    ///
    /// Validates the settings object and applies it to the system
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
            settings.language_code.clone_from(&prev.language_code);
        }

        settings
    }

    ///
    /// Update the registered shortcut
    pub fn update_shortcut(&self, new: &Shortcut, prev: &Shortcut) -> Result<(), Error> {
        if prev == new {
            Ok(())
        } else {
            let shortcut_controller = ShortcutController(self.0.clone());
            shortcut_controller.register(new)?;
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

    pub fn update_start_with_os(&self, new: bool, prev: bool) -> Result<(), Error> {
        if prev == new {
            return Ok(());
        } else if new {
            self.0.autolaunch().enable()?;
        } else {
            self.0.autolaunch().disable()?;
        }
        Ok(())
    }

    pub fn update_language_code(&self, new: &str, prev: &str) -> Result<(), Error> {
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
    pub fn create_all(&self) -> Result<(), Error> {
        Ok(fs::create_dir_all(self.read()?.ext_dir())?)
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_config_dir(&self) -> Result<(), Error> {
        FsUtils::open_dir(self.read()?.root_dir())?;
        Ok(())
    }

    ///
    /// Open the config directory in the system file explorer
    pub fn open_ext_dir(&self) -> Result<(), Error> {
        FsUtils::open_dir(&self.read()?.ext_dir())?;
        Ok(())
    }

    ///
    /// Attempt to write settings to a file
    pub fn save(&self) -> Result<(), Error> {
        debug!(self.0.clone(), "save-settings");
        let settings = self.read()?;
        let s = serde_json::to_string_pretty(&settings).expect("could not serialize settings");

        let mut file = File::create(settings.filename())?;
        file.write_all(s.as_bytes())?;
        Ok(())
    }

    ///
    /// Attempt to load settings from a file
    pub fn load(&self) -> Result<(), Error> {
        let contents = std::fs::read_to_string(self.read()?.filename())?;
        let settings = serde_json::from_str::<Settings>(&contents)?;
        self.write(&settings)?;
        Ok(())
    }

    ///
    /// Attempt to add a new extension
    pub fn add_extension(&self, filename: &Path) -> Result<(), Error> {
        FsUtils::copy(filename, &self.read()?.ext_dir())?;
        Ok(())
    }

    ///
    /// Attempt to delete an extension
    pub fn del_extension(&self, filename: &Path) -> Result<(), Error> {
        FsUtils::delete(filename, &self.read()?.ext_dir())?;
        Ok(())
    }

    pub fn set_root(&self, root_dir: &Path) -> Result<(), Error> {
        self.state().mutate(|settings| {
            settings.set_root(root_dir.to_owned());
            self.emit(settings)?;
            self.save()?;
            Ok(())
        })
    }

    pub fn root(&self) -> Result<std::path::PathBuf, Error> {
        self.read().map(|settings| settings.root_dir().to_owned())
    }
}
