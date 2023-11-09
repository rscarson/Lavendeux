// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

pub mod bugcheck;
pub mod config;
pub mod controllers;
pub mod managed_value;
pub mod models;

mod commands;

use controllers::{
    BlacklistController, Controller, DebugController, DebugableResult, ExtensionsController,
    LanguageController, ParserController, SettingsController, ShortcutController,
};
use controllers::{HistoryController, TrayController};

use managed_value::ManagedValue;

use tauri::tray::ClickType;
use tauri::Manager;
use tauri::WindowEvent;
use tauri_plugin_notification::NotificationExt;

fn main() {
    let app = tauri::Builder::default()
        //
        // Managed Values
        .manage(SettingsController::new_managed())
        .manage(LanguageController::new_managed())
        .manage(HistoryController::new_managed())
        .manage(BlacklistController::new_managed())
        .manage(ExtensionsController::new_managed())
        .manage(ParserController::new_managed())
        .manage(DebugController::new_managed())
        //
        // Plugins
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_notification::init())
        .plugin(tauri_plugin_clipboard_manager::init())
        .plugin(tauri_plugin_shell::init())
        .plugin(
            tauri_plugin_global_shortcut::Builder::with_handler(|app, _| {
                ParserController::main_handler(app.clone());
            })
            .build(),
        )
        .plugin(tauri_plugin_single_instance::init(|_app, _argv, _cwd| {
            //    println!("{}, {argv:?}, {cwd}", app.package_info().name);
            //    app.emit_all("single-instance", Payload { args: argv, cwd })
            //        .unwrap();
        }))
        .plugin(tauri_plugin_autostart::init(
            tauri_plugin_autostart::MacosLauncher::LaunchAgent,
            None,
        ))
        .invoke_handler(tauri::generate_handler![
            commands::language::translate,
            commands::language::help_text,
            commands::language::list_languages,
            //
            commands::settings::open_config_dir,
            commands::settings::read_settings,
            commands::settings::write_settings,
            commands::settings::app_exit,
            //
            commands::history::read_history,
            commands::history::clear_history,
            commands::history::del_history,
            commands::history::export_history,
            //
            commands::blacklist::read_blacklist,
            commands::blacklist::disable_extension,
            commands::blacklist::enable_extension,
            //
            commands::extensions::open_ext_dir,
            commands::extensions::reload_extensions,
            commands::extensions::read_extensions,
            commands::extensions::add_extension,
            commands::extensions::del_extension,
            //
            commands::debug::activate_debug,
            commands::debug::read_debug
        ])
        .setup(|app| {
            let handle = app.handle();

            // Updater
            //tauri::async_runtime::spawn(async move {
            //    let response = handle.updater_builder().build().unwrap().check().await;
            //});

            //
            // Attempt to create directories needed by the app
            if let Err(_) = config::ConfigManager::init(handle.clone()) {
                bugcheck::fatal(handle.clone(), "Could not write to settings");
            }

            //
            // Try to load up the settings - it's ok if this fails, we'll just end up with the defaults
            let (settings, blacklist) = config::ConfigManager::load(handle.clone());
            SettingsController(handle.clone())
                .write(&settings)
                .debug_ok(&handle, "write-settings");
            BlacklistController(handle.clone())
                .write(&blacklist)
                .debug_ok(&handle, "write-blacklist");

            //
            // Register default shortcut
            if ShortcutController(handle.clone())
                .register(&settings.shortcut)
                .is_err()
            {
                bugcheck::general(
                    handle.clone(),
                    "Could not register shortcut",
                    "Shortcut is not valid, please verify settings",
                );
            }

            //
            // Language settings
            if LanguageController(handle.clone())
                .set_language(&settings.language_code)
                .is_err()
            {
                bugcheck::general(
                    handle.clone(),
                    "Could not set language",
                    "Language code is invalid, please verify settings",
                );
            }

            //
            // Load extensions
            //
            if let Err(e) = ExtensionsController(handle.clone()).reload() {
                bugcheck::general(handle.clone(), "Could not load extensions", &e.to_string());
            }

            //
            // Run startup sequence
            if !settings.start_script.trim().is_empty() {
                let snippet = ParserController(handle.clone()).parse(&settings.start_script);
                if snippet.is_err() {
                    bugcheck::parser(handle.clone(), &snippet.result.to_string());
                }
            }

            //
            // Activate tray icon
            if TrayController(handle.clone()).init().is_err() {
                bugcheck::fatal(handle.clone(), "Could not create tray icon - exiting");
            }

            //
            // Show ready msg
            app.notification()
                .builder()
                .title("Lavendeux is running")
                .body("Highlight some text, and use CTRL-Space to start parsing!")
                .action_type_id("show_history")
                .show()
                .debug_ok(&handle, "notify");
            Ok(())
        })
        .build(tauri::generate_context!())
        .expect("error while running tauri application");

    app.run(|_app, event| {
        match event {
            tauri::RunEvent::Exit => {}

            // Keep the event loop running even if all windows are closed
            // This allow us to catch system tray events when there is no window
            tauri::RunEvent::ExitRequested { api, .. } => {
                api.prevent_exit();
            }

            // Window specific events
            tauri::RunEvent::WindowEvent { label, event, .. } => match event {
                WindowEvent::CloseRequested { api, .. } => {
                    api.prevent_close();
                    if let Some(window) = _app.get_window(&label) {
                        window.hide().debug_ok(&_app, "window-hide");
                    };

                    if label == "debug" {
                        DebugController(_app.clone()).deactivate();
                    }
                }
                _ => {}
            },
            tauri::RunEvent::Ready => {}
            tauri::RunEvent::MainEventsCleared => {}
            tauri::RunEvent::MenuEvent(_) => {}

            tauri::RunEvent::TrayIconEvent(e) => {
                if e.click_type == ClickType::Double {
                    if let Some(window) = _app.get_window("main") {
                        window.show().debug_ok(&_app, "window-show");
                        window.set_focus().debug_ok(&_app, "window-focus");
                    }
                }
            }

            _ => {}
        }
    });
}
