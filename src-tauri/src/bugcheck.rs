use tauri::AppHandle;
use tauri_plugin_dialog::DialogExt;
use tauri_plugin_notification::NotificationExt;

use crate::{
    controllers::{Controller, DebugableResult, SettingsController},
    debug,
};

///
/// Unrecoverable error that required quitting the software
pub fn fatal(app: AppHandle, error: &str) {
    app.dialog()
        .message(error)
        .ok_button_label("exit")
        .title("Fatal Error!")
        .show(move |_| {
            app.exit(1);
        });
}

///
/// Normal error that may be shown to the user
pub fn parser(app: AppHandle, error: &str) {
    let settings = SettingsController(app.clone()).read().unwrap_or_default();
    debug!(app.clone(), "Parser Error: {}", error);
    if settings.display_errors {
        app.notification()
            .builder()
            .title("Parser Error")
            .body(error)
            .show()
            .debug_ok(&app, "notify");
    }
}

pub fn general(app: AppHandle, title: &str, error: &str) {
    debug!(app.clone(), "{}: {}", title, error);
    app.notification()
        .builder()
        .title(title)
        .body(error)
        .show()
        .debug_ok(&app, "notify");
}
