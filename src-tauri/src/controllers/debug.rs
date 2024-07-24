use super::Controller;
use crate::{debug, error::Error, managed_value::ManagedValue, models::debug::DebugModel};
use tauri::{AppHandle, Manager, State};

pub struct DebugController(pub AppHandle);
impl Controller<DebugModel> for DebugController {
    const EVENT_NAME: &'static str = "updated-debug";

    fn new_managed() -> Result<crate::ManagedValue<DebugModel>, Error> {
        Ok(ManagedValue::new(DebugModel::default()))
    }

    fn state(&self) -> State<ManagedValue<DebugModel>> {
        self.0.state::<ManagedValue<DebugModel>>()
    }

    fn read(&self) -> Result<DebugModel, Error> {
        self.state().clone_inner()
    }

    fn borrow(&self) -> Result<std::sync::MutexGuard<'_, DebugModel>, Error> {
        self.state().inner().read()
    }

    fn write(&self, value: &DebugModel) -> Result<DebugModel, Error> {
        self.state().write(value.clone())?;
        self.emit(value)?;
        Ok(value.clone())
    }

    fn emit(&self, value: &DebugModel) -> Result<(), Error> {
        self.0.emit(Self::EVENT_NAME, value.clone())?;
        Ok(())
    }
}

impl DebugController {
    pub fn activate(&self) -> Result<(), Error> {
        debug!(self.0.clone(), "Debug active");
        if let Some(window) = self.0.get_webview_window("debug") {
            window.show()?;
            window.center()?;
            window.request_user_attention(Some(tauri::UserAttentionType::Informational))?;
        } else {
            eprintln!("No debug window found");
        }

        Ok(())
    }

    pub fn debug(&self, s: String) {
        self.state()
            .mutate(|out| {
                out.debug(s);
                self.emit(out)
            })
            .ok();
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
