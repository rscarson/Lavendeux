use super::{
    BlacklistController, Controller, DebugableResult, LanguageController, ParserController,
};
use crate::{
    bugcheck, config::ConfigManager, debug, managed_value::ManagedValue,
    models::extension::Extensions,
};
use std::path::Path;
use tauri::{AppHandle, Manager, State};

pub struct ExtensionsController(pub AppHandle);
impl Controller<Extensions> for ExtensionsController {
    const EVENT_NAME: &'static str = "updated-extensions";

    fn new_managed() -> ManagedValue<Extensions> {
        ManagedValue::new(Extensions::default())
    }

    fn state(&self) -> State<ManagedValue<Extensions>> {
        self.0.state::<ManagedValue<Extensions>>()
    }

    fn read(&self) -> Option<Extensions> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, Extensions>> {
        self.state().inner().read()
    }

    fn write(&self, value: &Extensions) -> Result<Extensions, String> {
        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        self.emit(&value);

        Ok(value.clone())
    }

    fn emit(&self, value: &Extensions) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl ExtensionsController {
    pub fn add(&self, filename: &str) -> Option<Extensions> {
        if let Ok(_) = ConfigManager::add_extension(Path::new(filename), self.0.clone()) {
            BlacklistController(self.0.clone()).enable(filename.to_string());
            self.reload().debug_ok(&self.0, "reload-extensions")
        } else {
            None
        }
    }

    pub fn remove(&self, filename: &str) -> Option<Extensions> {
        if let Ok(_) = ConfigManager::del_extension(Path::new(filename), self.0.clone()) {
            BlacklistController(self.0.clone()).enable(filename.to_string());
            self.reload().debug_ok(&self.0, "reload-extensions")
        } else {
            None
        }
    }

    pub fn reload(&self) -> Result<Extensions, std::io::Error> {
        let disabled_extensions = BlacklistController(self.0.clone())
            .read()
            .unwrap_or_default();
        let disabled_err = LanguageController(self.0.clone())
            .translate("extensions\\lbl_disabled_by_user")
            .unwrap_or("Disabled by User".to_string());

        let ext_path = match ConfigManager::ext_dir(self.0.clone()).to_str() {
            Some(s) => s.to_string(),
            None => {
                bugcheck::general(
                    self.0.clone(),
                    "Filesystem Error",
                    "Extensions directory is invalid; unicode translation error",
                );
                ".".to_string()
            }
        };

        let mut extensions = Extensions::default();
        if let Some(mut parser) = ParserController(self.0.clone()).borrow() {
            for module in lavendeux_parser::rustyscript::Module::load_dir(&ext_path)? {
                let filepath = module.filename().to_string();
                let filename = ConfigManager::basename(&filepath).unwrap_or_default();

                // Do not attempt to load disabled extensions
                if disabled_extensions.contains(&filename) {
                    debug!(
                        self.0.clone(),
                        "Skipping disabled extension at {}", filename
                    );
                    extensions.insert(filename.clone(), Err(disabled_err.clone()));
                    continue;
                }

                // Not disabled - attempt to load the extension
                debug!(self.0.clone(), "Found an extension at {}", filename);
                let extension = parser.extensions.load(&filepath);
                extensions.insert(
                    filename.clone(),
                    extension.clone().or_else(|e| Err(e.to_string())),
                );
                if let Ok(ext) = extension.clone() {
                    parser.extensions.add(&filepath, ext)
                }
            }
        }

        self.write(&extensions)
            .debug_ok(&self.0, "write-extensions");
        Ok(extensions)
    }
}
