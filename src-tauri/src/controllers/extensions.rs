use super::{Controller, SettingsController};
use crate::{error::Error, managed_value::ManagedValue, FsUtils};
use std::path::{Path, PathBuf};
use tauri::{AppHandle, Manager, State};

pub struct ExtensionsController(pub AppHandle);
impl Controller<Vec<lavendeux_js::Extension>> for ExtensionsController {
    const EVENT_NAME: &'static str = "updated-extensions";

    fn new_managed() -> Result<crate::ManagedValue<Vec<lavendeux_js::Extension>>, Error> {
        Ok(ManagedValue::new(Vec::new()))
    }

    fn state(&self) -> State<ManagedValue<Vec<lavendeux_js::Extension>>> {
        self.0.state::<ManagedValue<Vec<lavendeux_js::Extension>>>()
    }

    fn read(&self) -> Result<Vec<lavendeux_js::Extension>, Error> {
        let ext = lavendeux_js::with_worker(|runtime| runtime.list_extensions());
        Ok(ext)
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, Vec<lavendeux_js::Extension>>, Error> {
        self.state().inner().read()
    }

    fn write(
        &self,
        value: &Vec<lavendeux_js::Extension>,
    ) -> Result<Vec<lavendeux_js::Extension>, Error> {
        Ok(value.clone())
    }

    fn emit(&self, value: &Vec<lavendeux_js::Extension>) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, value.clone())?;
        Ok(())
    }
}

impl ExtensionsController {
    fn extensions_dir(&self) -> Result<PathBuf, Error> {
        Ok(SettingsController(self.0.clone()).read()?.ext_dir())
    }

    pub fn add(&self, path: &str) -> Result<lavendeux_js::Extension, Error> {
        // Copy the extension to the extensions directory
        let ext_dir = self.extensions_dir()?;
        FsUtils::copy(Path::new(path), ext_dir.as_path())?;

        // Get module
        let module = lavendeux_js::rustyscript::Module::load(path)?;

        // Load the extension
        let ext = lavendeux_js::with_worker(|runtime| runtime.add_extension(module))?;
        Ok(ext)
    }

    pub fn remove(&self, id: usize) -> Result<(), Error> {
        lavendeux_js::with_worker(|runtime| {
            if let Some(extension) = runtime.get_extension(id) {
                runtime.del_extension(id);

                let path = extension.module().filename();

                // Remove the extension from the filesystem
                let ext_dir = self.extensions_dir()?;
                FsUtils::delete(Path::new(path), ext_dir.as_path())?;
            }

            Ok(())
        })
    }

    pub fn list(&self) -> Vec<lavendeux_js::Extension> {
        let mut e = lavendeux_js::with_worker(|runtime| runtime.list_extensions());
        e.sort_by(|a, b| a.metadata().name.cmp(&b.metadata().name));
        e
    }

    pub fn load(&self) -> Result<(), Error> {
        // Get all modules in the extensions directory
        let ext_dir = self.extensions_dir()?;
        let ext_dir = ext_dir.to_string_lossy().into_owned();
        let modules = lavendeux_js::rustyscript::Module::load_dir(&ext_dir)?;

        // Load the modules
        lavendeux_js::with_worker(|runtime| {
            for module in modules {
                runtime.add_extension(module).ok();
            }
        });

        Ok(())
    }
}
