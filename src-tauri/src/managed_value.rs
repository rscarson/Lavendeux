use std::sync::{Mutex, MutexGuard};

use tauri::{AppHandle, Manager};

pub struct ManagedValue<T>(Mutex<T>);
impl<T> ManagedValue<T> {
    pub fn new(value: T) -> Self {
        Self(Mutex::new(value))
    }

    ///
    /// Attempt to read a value out of the instance
    pub fn read(&self) -> Option<MutexGuard<T>> {
        self.0.lock().ok()
    }

    ///
    /// Attempt to replace the value in the instance
    pub fn write(&self, value: T) -> Result<(), ()> {
        if let Ok(mut lock) = self.0.lock() {
            *lock = value;
            Ok(())
        } else {
            Err(())
        }
    }

    pub fn clone_inner(&self) -> Option<T>
    where
        T: std::marker::Send + 'static + Clone,
    {
        self.0.lock().and_then(|s| Ok(s.clone())).ok()
    }

    ///
    /// Attempt to get a clone of the inner value of a state
    pub fn clone_from(app: &AppHandle) -> Option<T>
    where
        T: std::marker::Send + 'static + Clone,
    {
        let state = app.state::<ManagedValue<T>>();
        state.inner().clone_inner()
    }

    ///
    ///  Get a clone of the inner value of a state, or
    /// the default value for the type
    pub fn cloned_or_default(app: &AppHandle) -> T
    where
        T: std::marker::Send + 'static + Clone + Default,
    {
        Self::clone_from(app).unwrap_or_default()
    }
}
