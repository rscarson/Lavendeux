use crate::error::Error;
use std::sync::{Mutex, MutexGuard};
use tauri::{AppHandle, Manager};

pub struct ManagedValue<T>(Mutex<T>);
impl<T> ManagedValue<T> {
    pub fn new(value: T) -> Self {
        Self(Mutex::new(value))
    }

    ///
    /// Run a function that mutates the inner value of the state
    pub fn mutate<F, R>(&self, f: F) -> Result<R, Error>
    where
        F: FnOnce(&mut T) -> Result<R, Error>,
    {
        let mut lock = self.0.try_lock()?;
        f(&mut *lock)
    }

    ///
    /// Attempt to read a value out of the instance
    pub fn read(&self) -> Result<MutexGuard<T>, Error> {
        Ok(self.0.try_lock()?)
    }

    ///
    /// Attempt to replace the value in the instance
    pub fn write(&self, value: T) -> Result<(), Error> {
        let mut lock = self.0.try_lock()?;
        *lock = value;
        Ok(())
    }

    pub fn clone_inner(&self) -> Result<T, Error>
    where
        T: std::marker::Send + 'static + Clone,
    {
        Ok(self.0.try_lock()?.clone())
    }

    ///
    /// Attempt to get a clone of the inner value of a state
    pub fn clone_from(app: &AppHandle) -> Result<T, Error>
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
