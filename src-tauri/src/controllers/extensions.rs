use super::{Controller, DebugableResult, SettingsController};
use crate::{bugcheck, debug, managed_value::ManagedValue, FsUtils};
use lavendeux_js::Error;
use std::path::{Path, PathBuf};
use tauri::{AppHandle, Manager, State};

pub struct ExtensionsController(pub AppHandle);
impl Controller<Vec<lavendeux_js::Extension>> for ExtensionsController {
    const EVENT_NAME: &'static str = "updated-extensions";

    fn new_managed() -> Result<crate::ManagedValue<Vec<lavendeux_js::Extension>>, String> {
        Ok(ManagedValue::new(Vec::new()))
    }

    fn state(&self) -> State<ManagedValue<Vec<lavendeux_js::Extension>>> {
        self.0.state::<ManagedValue<Vec<lavendeux_js::Extension>>>()
    }

    fn read(&self) -> Option<Vec<lavendeux_js::Extension>> {
        lavendeux_js::with_worker(|runtime| {
            let extensions = runtime.list_extensions();
            Ok(extensions)
        })
        .debug_ok(&self.0, "read-extensions")
    }

    fn borrow(
        &self,
    ) -> std::option::Option<std::sync::MutexGuard<'_, Vec<lavendeux_js::Extension>>> {
        self.state().inner().read()
    }

    fn write(
        &self,
        value: &Vec<lavendeux_js::Extension>,
    ) -> Result<Vec<lavendeux_js::Extension>, String> {
        Ok(value.clone())
    }

    fn emit(&self, value: &Vec<lavendeux_js::Extension>) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl ExtensionsController {
    fn extensions_dir(&self) -> Result<PathBuf, Error> {
        match SettingsController(self.0.clone()).read() {
            Some(config) => Ok(config.ext_dir()),
            None => {
                bugcheck::general(
                    self.0.clone(),
                    "Could not delete an extension",
                    "could not read config",
                );
                Err(Error::Runtime("could not read config".to_string()))
            }
        }
    }

    pub fn add(&self, path: &str) -> Result<lavendeux_js::Extension, Error> {
        // Copy the extension to the extensions directory
        let ext_dir = self.extensions_dir()?;
        if let Err(e) = FsUtils::copy(Path::new(path), ext_dir.as_path()) {
            bugcheck::general(
                self.0.clone(),
                "Could not write an extension",
                &format!("could not write extension: {}", e),
            );
            return Err(Error::Runtime("could not write extension".to_string()));
        }

        // Get module
        let module = lavendeux_js::rustyscript::Module::load(path)?;

        // Load the extension
        lavendeux_js::with_worker(|runtime| runtime.add_extension(module))
    }

    pub fn remove(&self, id: usize) {
        lavendeux_js::with_worker(|runtime| {
            if let Some(extension) = runtime.get_extension(id) {
                runtime.del_extension(id);
                let path = extension.module().filename();

                // Remove the extension from the filesystem
                let ext_dir = self.extensions_dir()?;
                if let Err(e) = FsUtils::delete(Path::new(path), ext_dir.as_path()) {
                    bugcheck::general(
                        self.0.clone(),
                        "Could not delete an extension",
                        &format!("could not delete extension: {}", e),
                    );
                }
            }
            Ok(())
        })
        .ok();
    }

    pub fn list(&self) -> Vec<lavendeux_js::Extension> {
        debug!(self.0.clone(), "list-extensions");
        let mut e = lavendeux_js::with_worker(|runtime| {
            let extensions = runtime.list_extensions();
            Ok(extensions)
        })
        .unwrap_or_default();
        e.sort_by(|a, b| a.metadata().name.cmp(&b.metadata().name));
        e
    }

    pub fn load(&self) -> Result<(), String> {
        debug!(self.0.clone(), "load-extensions");
        // Get all modules in the extensions directory
        let ext_dir = self
            .extensions_dir()
            .map_err(|e| e.as_highlighted(Default::default()))?;
        let ext_dir = ext_dir.to_str().ok_or("Path was not valid UTF-8")?;
        let modules =
            lavendeux_js::rustyscript::Module::load_dir(ext_dir).map_err(|e| e.to_string())?;

        // Load the modules
        lavendeux_js::with_worker(|runtime| {
            for module in modules {
                runtime.add_extension(module).ok();
            }
            Ok(())
        })
        .ok();

        Ok(())
    }
}
