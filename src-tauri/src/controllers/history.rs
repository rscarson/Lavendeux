use crate::{
    error::Error,
    managed_value::ManagedValue,
    models::history::{History, Snippet},
};

use super::{Controller, TrayController};
use tauri::{AppHandle, Manager, State};

pub struct HistoryController(pub AppHandle);
impl Controller<History> for HistoryController {
    const EVENT_NAME: &'static str = "history-updated";

    fn new_managed() -> Result<ManagedValue<History>, Error> {
        Ok(ManagedValue::new(History::default()))
    }

    fn state(&self) -> State<ManagedValue<History>> {
        self.0.state::<ManagedValue<History>>()
    }

    fn read(&self) -> Result<History, Error> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, History>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &History) -> Result<History, Error> {
        self.state().write(value.clone())?;
        self.emit(value)?;
        TrayController(self.0.clone()).update()?;
        Ok(value.clone())
    }

    fn emit(&self, value: &History) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, value.clone())?;
        Ok(())
    }
}

impl HistoryController {
    ///
    /// Empty the history
    pub fn clear(&mut self) -> Result<(), Error> {
        self.state().mutate(|history| {
            history.clear();
            self.emit(history)?;
            Ok(())
        })
    }

    ///
    /// Remove an item from the history
    pub fn remove(&mut self, id: usize) -> Result<(), Error> {
        self.state().mutate(|history| {
            history.remove(id);
            self.emit(history)?;
            Ok(())
        })
    }

    ///
    /// Add an item to the history tab
    pub fn add(&mut self, snippet: Snippet) -> Result<(), Error> {
        self.state().mutate(|history| {
            history.add(snippet);
            self.emit(history)?;
            Ok(())
        })
    }
}
