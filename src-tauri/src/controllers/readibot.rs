use super::Controller;
use crate::{error::Error, managed_value::ManagedValue};
use tauri::{AppHandle, Manager};

pub struct ReadibotController(pub AppHandle);
impl Controller<bool> for ReadibotController {
    const EVENT_NAME: &'static str = "ready";

    fn new_managed() -> Result<crate::ManagedValue<bool>, Error> {
        Ok(ManagedValue::new(false))
    }

    fn state(&self) -> tauri::State<crate::ManagedValue<bool>> {
        self.0.state::<crate::ManagedValue<bool>>()
    }

    fn read(&self) -> Result<bool, Error> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, bool>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &bool) -> Result<bool, Error> {
        self.state().write(*value)?;
        self.emit(value)?;
        Ok(*value)
    }

    fn emit(&self, value: &bool) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, *value)?;
        Ok(())
    }
}

impl ReadibotController {
    pub fn is_ready(&self) -> bool {
        self.read().unwrap_or(false)
    }

    pub fn set_ready(&self, value: bool) -> bool {
        self.write(&value).unwrap_or(false)
    }
}
