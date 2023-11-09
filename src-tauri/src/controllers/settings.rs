use tauri::{AppHandle, Manager, State};
use tauri_plugin_autostart::ManagerExt;

use crate::{
    bugcheck,
    config::ConfigManager,
    managed_value::ManagedValue,
    models::{settings::Settings, shortcut::Shortcut},
};

use super::{Controller, DebugableResult, LanguageController, ShortcutController, TrayController};

pub struct SettingsController(pub AppHandle);
impl Controller<Settings> for SettingsController {
    const EVENT_NAME: &'static str = "updated-settings";
    fn new_managed() -> ManagedValue<Settings> {
        ManagedValue::new(Settings::default())
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

        // Run handlers
        value = self.update(&value, &last_known_good);

        //
        // All is well - emit new settings
        //

        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        ConfigManager::save(self.0.clone()).debug_ok(&self.0, "save-config");
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
                    "error registering hotkey; manager is out of sync! Please report this issue",
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
}
