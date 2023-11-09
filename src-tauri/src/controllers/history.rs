use crate::{
    managed_value::ManagedValue,
    models::history::{History, Snippet},
};

use super::{Controller, DebugableResult, TrayController};
use tauri::{AppHandle, Manager, State};

pub struct HistoryController(pub AppHandle);
impl Controller<History> for HistoryController {
    const EVENT_NAME: &'static str = "history-updated";

    fn new_managed() -> ManagedValue<History> {
        ManagedValue::new(History::default())
    }

    fn state(&self) -> State<ManagedValue<History>> {
        self.0.state::<ManagedValue<History>>()
    }

    fn read(&self) -> Option<History> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Option<std::sync::MutexGuard<'_, History>> {
        self.state().inner().read()
    }

    fn write(&self, value: &History) -> Result<History, String> {
        self.state()
            .write(value.clone())
            .or(Err("could not write to state".to_string()))?;
        self.emit(value);
        TrayController(self.0.clone())
            .update()
            .debug_ok(&self.0, "update-tray");
        Ok(value.clone())
    }

    fn emit(&self, value: &History) {
        self.0
            .emit(Self::EVENT_NAME, value.clone())
            .debug_ok(&self.0, "emit-event");
    }
}

impl HistoryController {
    ///
    /// Empty the history and return the result if successful
    pub fn clear(&mut self) -> Option<History> {
        if let Some(mut history) = self.read() {
            history.clear();
            self.write(&history).debug_ok(&self.0, "write-history")
        } else {
            None
        }
    }

    ///
    /// Remove an item from the history and return the result if successful
    pub fn remove(&mut self, id: usize) -> Option<History> {
        if let Some(mut history) = self.read() {
            history.remove(id);
            self.write(&history).debug_ok(&self.0, "write-history")
        } else {
            None
        }
    }

    ///
    /// Add an item to the history tab
    pub fn add(&mut self, snippet: Snippet) -> Option<History> {
        if let Some(mut history) = self.read() {
            history.add(snippet);
            self.write(&history).debug_ok(&self.0, "write-history")
        } else {
            None
        }
    }
}
