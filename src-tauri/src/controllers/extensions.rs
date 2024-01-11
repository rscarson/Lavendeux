use super::{
    BlacklistController, ConfigController, Controller, DebugableResult, LanguageController,
    ParserController,
};
use crate::{bugcheck, debug, managed_value::ManagedValue, models::extension::Extensions, FsUtils};
use lavendeux_parser2::rustyscript;
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
        if ConfigController(self.0.clone()).add_extension(Path::new(filename)) {
            BlacklistController(self.0.clone()).enable(filename.to_string());
            self.reload().debug_ok(&self.0, "reload-extensions")
        } else {
            None
        }
    }

    pub fn remove(&self, filename: &str) -> Option<Extensions> {
        if ConfigController(self.0.clone()).del_extension(Path::new(filename)) {
            BlacklistController(self.0.clone()).enable(filename.to_string());
            self.reload().debug_ok(&self.0, "reload-extensions")
        } else {
            None
        }
    }

    pub fn reload(&self) -> Result<Extensions, String> {
        let disabled_extensions = BlacklistController(self.0.clone())
            .read()
            .unwrap_or_default();
        let disabled_err = LanguageController(self.0.clone())
            .translate("extensions\\lbl_disabled_by_user")
            .unwrap_or("Disabled by User".to_string());

        let config = ConfigController(self.0.clone())
            .read()
            .ok_or("could not lock settings")?;

        let ext_dir = config
            .ext_dir()
            .to_str()
            .or_else(|| {
                bugcheck::general(
                    self.0.clone(),
                    "Filesystem Error",
                    "Extensions directory is invalid; unicode translation error",
                );
                Some(".")
            })
            .unwrap()
            .to_string();

        let mut extensions = Extensions::default();
        if let Some(mut parser) = ParserController(self.0.clone()).borrow() {
            parser.unload_all_extensions();
            for module in rustyscript::Module::load_dir(&ext_dir).or_else(|e| Err(e.to_string()))? {
                let filepath = module.filename().to_string();
                let filename = FsUtils::basename(&filepath).unwrap_or_default();

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
                let extension = parser
                    .load_extension(&filepath)
                    .or_else(|e| Err(e.to_string()));
                extensions.insert(filename.clone(), extension.clone());
            }
        }

        println!(
            "Reloaded extensions: {} extensions loaded",
            extensions.len()
        );
        self.write(&extensions)
            .debug_ok(&self.0, "write-extensions");
        Ok(extensions)
    }
}
