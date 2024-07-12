use std::sync::{Mutex, MutexGuard};

use tauri::{AppHandle, Manager};

pub struct ManagedValue<T>(Mutex<T>);
impl<T> ManagedValue<T> {
    pub fn new(value: T) -> Self {
        Self(Mutex::new(value))
    }

    ///
    /// Run a function that mutates the inner value of the state
    pub fn mutate<F>(&self, f: F) -> Result<(), String>
    where
        F: FnOnce(&mut T),
    {
        if let Ok(mut lock) = self.0.try_lock() {
            f(&mut *lock);
            Ok(())
        } else {
            Err("could not lock state".to_string())
        }
    }

    ///
    /// Attempt to read a value out of the instance
    pub fn read(&self) -> Option<MutexGuard<T>> {
        self.0.try_lock().ok()
    }

    ///
    /// Attempt to replace the value in the instance
    pub fn write(&self, value: T) -> Result<(), ()> {
        if let Ok(mut lock) = self.0.try_lock() {
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
        self.0.try_lock().and_then(|s| Ok(s.clone())).ok()
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
