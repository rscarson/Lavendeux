use super::{ClipboardController, Controller, DebugableResult, SettingsController};
use crate::models::tray::MyMenuBuilder;
use tauri::{
    menu::{MenuBuilder, MenuEvent},
    tray::TrayIcon,
    AppHandle, Manager, Wry,
};

pub struct TrayController(pub AppHandle);
impl TrayController {
    pub fn init(&self) -> Result<(), tauri::Error> {
        self.handle().on_menu_event(Self::handle_event);
        self.update()
    }

    fn handle(&self) -> TrayIcon<Wry> {
        self.0.tray_by_id("main").unwrap()
    }

    pub fn update(&self) -> Result<(), tauri::Error> {
        let menu = MenuBuilder::new(&self.0).add_all(&self.0).build()?;
        self.handle().set_menu(Some(menu))
    }

    pub fn handle_event(app: &AppHandle, event: MenuEvent) {
        match event.id().0.as_str() {
            "settings" => {
                if let Some(window) = app.get_webview_window("main") {
                    window
                        .emit("tab-request", "/settings")
                        .debug_ok(&app, "tab-request-settings");
                    window.show().debug_ok(&app, "window-show");
                    window.set_focus().debug_ok(&app, "window-focus");
                }
            }

            "history" => {
                if let Some(window) = app.get_webview_window("main") {
                    window
                        .emit("tab-request", "/history")
                        .debug_ok(&app, "tab-request-history");
                    window.show().debug_ok(&app, "window-show");
                    window.set_focus().debug_ok(&app, "window-focus");
                }
            }

            "enhanced_clipboard" => {
                let settings_controller = SettingsController(app.clone());
                settings_controller
                    .state()
                    .mutate(|settings| {
                        settings.enhanced_clipboard = !settings.enhanced_clipboard;
                        settings_controller.emit(settings);
                        settings_controller.save();
                    })
                    .debug_ok(&app, "settings-write-ecm");
            }

            "display_errors" => {
                let settings_controller = SettingsController(app.clone());
                settings_controller
                    .state()
                    .mutate(|settings| {
                        settings.display_errors = !settings.display_errors;
                        settings_controller.emit(settings);
                        settings_controller.save();
                    })
                    .debug_ok(&app, "settings-write-errormode");
            }

            "quit" => {
                app.exit(0);
            }

            s => {
                // Assume it's an expression and copy it
                ClipboardController(app.clone()).write(s);
            }
        }
    }
}
