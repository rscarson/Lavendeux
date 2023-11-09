use std::fmt::{Debug, Display};

use tauri::{AppHandle, Manager, State};

use super::Controller;
use crate::{debug, managed_value::ManagedValue, models::debug::DebugOutput};

pub struct DebugController(pub AppHandle);
impl Controller<DebugOutput> for DebugController {
    const EVENT_NAME: &'static str = "updated-debug";

    fn new_managed() -> ManagedValue<DebugOutput> {
        ManagedValue::new(DebugOutput::default())
    }

    fn state(&self) -> State<ManagedValue<DebugOutput>> {
        self.0.state::<ManagedValue<DebugOutput>>()
    }

    fn read(&self) -> Option<DebugOutput> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> std::option::Option<std::sync::MutexGuard<'_, DebugOutput>> {
        self.state().inner().read()
    }

    fn write(&self, value: &DebugOutput) -> Result<DebugOutput, String> {
        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        self.emit(&value);

        Ok(value.clone())
    }

    fn emit(&self, value: &DebugOutput) {
        self.0.emit(Self::EVENT_NAME, value.clone()).ok();
    }
}

impl DebugController {
    pub fn is_active(&self) -> bool {
        if let Some(inner) = self.read() {
            inner.is_some()
        } else {
            false
        }
    }

    pub fn deactivate(&self) {
        self.write(&None).ok();
    }

    pub fn activate(&self) {
        if self.is_active() {
            return;
        }
        self.write(&Some(Vec::default())).ok();
        debug!(self.0.clone(), "Debug active");
        if let Some(window) = self.0.get_window("debug") {
            window.show().debug_ok(&self.0, "show-debug");
        }
    }

    pub fn debug(&self, s: String) {
        if let Some(out) = self.read() {
            if let Some(mut inner) = out {
                inner.push(s);
                self.write(&Some(inner)).ok();
            }
        }
    }
}

#[macro_use]
mod debug_macros {
    #[macro_export]
    macro_rules! debug {
        ($app:expr, $f:literal, $($arg:expr),+) => {
            $crate::controllers::DebugController($app).debug(format!($f, $($arg),+))
        };

        ($app:expr, $f:literal) => {
            $crate::controllers::DebugController($app).debug($f.to_string())
        };
    }
}

pub trait DebugableResult<T> {
    fn debug_ok(self, app: &AppHandle, action: &str) -> Option<T>;
}

impl<T, E> DebugableResult<T> for Result<T, E>
where
    E: Display,
    T: Debug,
{
    fn debug_ok(self, app: &AppHandle, action: &str) -> Option<T> {
        if self.is_err() {
            debug!(app.clone(), "{}: {}", action, self.unwrap_err());
            None
        } else {
            self.ok()
        }
    }
}
