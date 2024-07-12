use std::fmt::{Debug, Display};

use tauri::{AppHandle, Manager, State};

use super::Controller;
use crate::{debug, managed_value::ManagedValue, models::debug::DebugModel};

pub struct DebugController(pub AppHandle);
impl Controller<DebugModel> for DebugController {
    const EVENT_NAME: &'static str = "updated-debug";

    fn new_managed() -> Result<crate::ManagedValue<DebugModel>, String> {
        Ok(ManagedValue::new(DebugModel::default()))
    }

    fn state(&self) -> State<ManagedValue<DebugModel>> {
        self.0.state::<ManagedValue<DebugModel>>()
    }

    fn read(&self) -> Option<DebugModel> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> std::option::Option<std::sync::MutexGuard<'_, DebugModel>> {
        self.state().inner().read()
    }

    fn write(&self, value: &DebugModel) -> Result<DebugModel, String> {
        self.state()
            .write(value.clone())
            .or(Err("unknown error".to_string()))?;
        self.emit(&value);

        Ok(value.clone())
    }

    fn emit(&self, value: &DebugModel) {
        self.0.emit(Self::EVENT_NAME, value.clone()).ok();
    }
}

impl DebugController {
    pub fn activate(&self) {
        debug!(self.0.clone(), "Debug active");
        if let Some(window) = self.0.get_webview_window("debug") {
            if let Err(e) = window.show() {
                println!("Error showing debug window: {:?}", e);
            } else {
                window.center().debug_ok(&self.0, "center-debug");
                window
                    .request_user_attention(Some(tauri::UserAttentionType::Informational))
                    .debug_ok(&self.0, "request-user-attention");
            }
        } else {
            println!("No debug window found");
        }
    }

    pub fn debug(&self, s: String) {
        self.state().mutate(|out| out.debug(s)).ok();
        if let Some(debug) = self.read() {
            self.emit(&debug)
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
