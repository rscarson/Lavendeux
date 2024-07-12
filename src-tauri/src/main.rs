// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

pub mod bugcheck;
pub mod controllers;
pub mod managed_value;
pub mod models;

pub mod fs;
use std::path::Path;

use commands::debug;
pub use fs::FsUtils;

mod commands;

use controllers::{
    Controller, DebugController, DebugableResult, ExtensionsController, LanguageController, ParserController, ReadibotController, SettingsController, ShortcutController
};
use controllers::{HistoryController, TrayController};

use managed_value::ManagedValue;

use tauri::Manager;
use tauri::WindowEvent;
use tauri_plugin_cli::CliExt;
use tauri_plugin_notification::NotificationExt;

macro_rules! manage_controller {
    ($controller:ident, $handle:ident) => {
        match $controller::new_managed() {
            Ok(controller) => {$handle.manage(controller);},
            Err(e) => bugcheck::fatal($handle.clone(), &format!("Could not register {}", stringify!($controller)), &e),
        }
    };
}

fn main() {
    let app = tauri::Builder::default()
        //
        // Plugins
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_notification::init())
        .plugin(tauri_plugin_clipboard_manager::init())
        .plugin(tauri_plugin_shell::init())
        .plugin(tauri_plugin_cli::init())
        .plugin(
            tauri_plugin_global_shortcut::Builder::new().with_handler(|app, _, event| {
                if event.state == tauri_plugin_global_shortcut::ShortcutState::Pressed {
                    ParserController::main_handler(app.clone(), event);
                }
            })
            .build(),
        )
        .plugin(tauri_plugin_single_instance::init(|app, _argv, _cwd| {
            bugcheck::general(
                app.clone(), 
                "Lavendeux is already running!", 
                "Check the system tray icons - in Windows, that's at the bottom right of the taskbar!"
            );
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
            commands::settings::restart_parser,
            //
            commands::history::read_history,
            commands::history::clear_history,
            commands::history::del_history,
            commands::history::export_history,
            //
            commands::extensions::open_ext_dir,
            commands::extensions::read_extensions,
            commands::extensions::add_extension,
            commands::extensions::del_extension,
            //
            commands::debug::activate_debug,
            commands::debug::read_debug,
            //
            commands::readibot::is_ready,
        ])
        .manage(ReadibotController::new_managed().unwrap())
        .manage(LanguageController::new_managed().unwrap())
        .setup(move |app| {
            let handle = app.handle();

            manage_controller!(DebugController, handle);
            debug!(handle.clone(), "load-controllers");
            
            manage_controller!(SettingsController, handle);
            manage_controller!(HistoryController, handle);
            manage_controller!(ExtensionsController, handle);
            manage_controller!(ParserController, handle);

            // Set the default root to the config directory
            debug!(handle.clone(), "set-default-root");
            SettingsController(handle.clone()).state().mutate(|settings| {
                let path = settings.default_root(handle.clone());
                settings.set_root(path);
            }).ok();

            debug!(handle.clone(), "process-cli-args");
            if let Ok(matches) = handle.cli().matches() {
                let debug_mode = matches.args.get("debug").unwrap().occurrences > 0;
                let config_dir = matches.args.get("config-dir").unwrap();

                if debug_mode {
                    debug!(handle.clone(), "activate-debug");
                    DebugController(handle.clone()).activate();
                    DebugController(handle.clone()).state().mutate(|out| out.debug_type = models::debug::DebugType::FileSystem).ok();
                }

                if config_dir.value.is_string() {
                    debug!(handle.clone(), "set-root-dir");
                    let path = Path::new(config_dir.value.as_str().unwrap()).to_owned();
                    SettingsController(handle.clone()).set_root(&path);
                }
            }

            //
            // Attempt to create directories needed by the app
            debug!(handle.clone(), "create-dirs");
            if let Err(_) = SettingsController(handle.clone()).create_all() {
                bugcheck::fatal(handle.clone(), 
                    "Could not write to settings", 
                    "Could not write to the configuration directory - do you have the right permissions?"
                );
            }

            //
            // Load extensions
            //
            debug!(handle.clone(), "load-extensions");
            if let Err(e) = ExtensionsController(handle.clone()).load() {
                bugcheck::general(handle.clone(), "Could not load extensions", &e.to_string());
            }

            //
            // Try to load up the settings - it's ok if this fails, we'll just end up with the defaults
            debug!(handle.clone(), "load-settings");
            SettingsController(handle.clone()).load();
            let settings = SettingsController(handle.clone())
                .read()
                .unwrap_or_default();

            //
            // Register default shortcut
            debug!(handle.clone(), "register-shortcut");
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
            debug!(handle.clone(), "set-language");
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
            // Run startup sequence
            debug!(handle.clone(), "run-start-script");
            if !settings.start_script.trim().is_empty() {
                let snippet = ParserController(handle.clone()).parse(&settings.start_script);
                if snippet.is_err() {
                    bugcheck::parser(handle.clone(), &snippet.result.to_string());
                }
            }

            //
            // Activate tray icon
            debug!(handle.clone(), "activate-tray");
            if TrayController(handle.clone()).init().is_err() {
                bugcheck::fatal(handle.clone(), 
                    "Could not create tray icon",
                    "Lavendeux relies on the system tray icon to function properly. Unable to continue."
                );
            }

            // Signal readiness to the frontend
            ReadibotController(handle.clone()).set_ready(true);

            //
            // Show ready msg
            debug!(handle.clone(), "show-ready-msg");
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
            tauri::RunEvent::Exit => {

            }

            // Keep the event loop running even if all windows are closed
            // This allow us to catch system tray events when there is no window
            tauri::RunEvent::ExitRequested { api, code, .. } => {
                match code {
                    Some(0) => {
                        // Do nothing
                    }

                    _ => api.prevent_exit(),
                }
            }

            // Window specific events
            tauri::RunEvent::WindowEvent { label, event, .. } => match event {
                WindowEvent::CloseRequested { api, .. } => {
                    api.prevent_close();
                    if let Some(window) = _app.get_webview_window(&label) {
                        window.hide().debug_ok(&_app, "window-hide");
                    };
                }
                _ => {}
            },
            tauri::RunEvent::Ready => {}
            tauri::RunEvent::MainEventsCleared => {}
            tauri::RunEvent::MenuEvent(_) => {}

            tauri::RunEvent::TrayIconEvent(tauri::tray::TrayIconEvent::Click{button, ..}) if button == tauri::tray::MouseButton::Left => {
                if let Some(window) = _app.get_webview_window("main") {
                    window.show().debug_ok(&_app, "window-show");
                    window.set_focus().debug_ok(&_app, "window-focus");
                }
            }

            _ => {}
        }
    });
}