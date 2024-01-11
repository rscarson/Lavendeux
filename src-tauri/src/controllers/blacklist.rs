use tauri::{AppHandle, Manager, State};

use super::{ConfigController, Controller, DebugableResult, ExtensionsController};
use crate::{managed_value::ManagedValue, models::extension::Blacklist, FsUtils};

pub struct BlacklistController(pub AppHandle);
impl Controller<Blacklist> for BlacklistController {
    const EVENT_NAME: &'static str = "updated-blacklist";

    fn new_managed() -> ManagedValue<Blacklist> {
        ManagedValue::new(Blacklist::default())
    }

    fn state(&self) -> State<ManagedValue<Blacklist>> {
        self.0.state::<ManagedValue<Blacklist>>()
    }

    fn read(&self) -> Option<Blacklist> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> std::option::Option<std::sync::MutexGuard<'_, Blacklist>> {
        self.state().inner().read()
    }

    fn write(&self, value: &Blacklist) -> Result<Blacklist, String> {
        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        ConfigController(self.0.clone()).save();
        self.emit(&value);

        Ok(value.clone())
    }

    fn emit(&self, value: &Blacklist) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl BlacklistController {
    pub fn disable(&self, filename: String) -> Option<Blacklist> {
        println!("disable: {}", filename);
        let filename = FsUtils::basename(&filename).unwrap_or_default();
        if let Some(mut blacklist) = self.read() {
            blacklist.insert(filename);
            let r = self.write(&blacklist).debug_ok(&self.0, "write-blacklist");
            ExtensionsController(self.0.clone())
                .reload()
                .debug_ok(&self.0, "reload-extensions");
            r
        } else {
            None
        }
    }

    pub fn enable(&self, filename: String) -> Option<Blacklist> {
        println!("enable: {}", filename);
        let filename = FsUtils::basename(&filename).unwrap_or_default();
        if let Some(mut blacklist) = self.read() {
            blacklist.remove(&filename);
            let r = self.write(&blacklist).debug_ok(&self.0, "write-blacklist");
            ExtensionsController(self.0.clone())
                .reload()
                .debug_ok(&self.0, "reload-extensions");
            r
        } else {
            None
        }
    }
}
