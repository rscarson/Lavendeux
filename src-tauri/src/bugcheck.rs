use tauri::AppHandle;
use tauri_plugin_dialog::{DialogExt, MessageDialogKind};
use tauri_plugin_notification::NotificationExt;

use crate::{
    controllers::{Controller, SettingsController},
    debug,
};

pub trait BugcheckedOk<T> {
    fn checked_ok(self, app: &AppHandle, title: &str) -> Option<T>;
}

impl<T, E> BugcheckedOk<T> for Result<T, E>
where
    E: std::fmt::Display,
{
    fn checked_ok(self, app: &AppHandle, title: &str) -> Option<T> {
        match self {
            Ok(t) => Some(t),
            Err(e) => {
                general(app.clone(), title, &format!("{}", e));
                None
            }
        }
    }
}

///
/// Unrecoverable error that required quitting the software
/// Appears as a dialog box, and exits the software
pub fn fatal(app: AppHandle, title: &str, error: &str) {
    app.dialog()
        .message(error)
        .kind(MessageDialogKind::Error)
        .ok_button_label("exit")
        .title(format!("Fatal Error: {title}"))
        .show(move |_| {
            app.exit(1);
        });
}

///
/// Normal error that may be shown to the user
/// but does not require quitting the software
/// Appears as a notification in the system tray
/// And relates to the parser
pub fn parser(app: AppHandle, error: &str) {
    let settings = SettingsController(app.clone()).read().unwrap_or_default();
    debug!(app.clone(), "Parser Error: {}", error);
    if settings.display_errors {
        app.notification()
            .builder()
            .title("Parser Error")
            .body(error)
            .show()
            .ok();
    }
}

///
/// General error that may be shown to the user
/// but does not require quitting the software
/// Appears as a notification in the system tray
pub fn general(app: AppHandle, title: &str, error: &str) {
    debug!(app.clone(), "{}: {}", title, error);
    app.notification()
        .builder()
        .title(title)
        .body(error)
        .show()
        .ok();
}
