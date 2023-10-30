// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

pub mod bugcheck;
pub mod config;
pub mod managed_value;
pub mod models;

use embedded_lang::embedded_language;
use embedded_lang::LanguageSet;
use lavendeux_parser::ParserState;
use managed_value::ManagedValue;

use models::extension;
use models::extension::Blacklist;
use models::extension::Extensions;
use models::extension::ManagedBlacklist;
use models::extension::ManagedExtensions;
use models::extension::ManagedParserState;
use models::language;
use models::language::ManagedTranslator;
use models::language::TranslatorManager;
use models::parser;
use models::settings;
use models::settings::Settings;
use models::snippet;
use tauri::Manager;
use tauri::WindowEvent;

fn main() {
    let mut translator = LanguageSet::new(
        "en",
        &[
            embedded_language!("../../language/en.lang.json"),
            embedded_language!("../../language/fr.lang.json"),
            embedded_language!("../../language/de.lang.json"),
            embedded_language!("../../language/ja.lang.json"),
            embedded_language!("../../language/zh-cn.lang.json"),
        ],
    );
    translator.set_fallback_language("en");

    let app = tauri::Builder::default()
        .manage(ManagedValue::new(settings::Settings::default()))
        .manage(ManagedValue::new(snippet::History::default()))
        .manage(ManagedExtensions::new(Extensions::default()))
        .manage(ManagedParserState::new(ParserState::new()))
        .manage(ManagedBlacklist::new(Blacklist::default()))
        .manage(ManagedTranslator::new(translator))
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_clipboard_manager::init())
        .plugin(tauri_plugin_shell::init())
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
            settings::open_config_dir,
            settings::read_settings,
            settings::write_settings,
            //
            extension::read_blacklist,
            extension::disable_extension,
            extension::enable_extension,
            extension::open_ext_dir,
            extension::reload_extensions,
            extension::read_extensions,
            extension::add_extension,
            extension::del_extension,
            //
            snippet::read_history,
            snippet::clear_history,
            snippet::del_history,
            snippet::export_history,
            //
            language::translate,
            language::list_languages,
        ])
        .setup(|app| {
            let handle = app.handle();

            // Load the gs handler plugin
            handle
                .plugin(
                    tauri_plugin_global_shortcut::Builder::with_handler(parser::shortcut_handler)
                        .build(),
                )
                .expect("could not start hotkey handler!");

            if let Err(_) = config::ConfigManager::init(handle.clone()) {
                bugcheck::fatal(handle.clone(), "Could not write to settings").ok();
            }

            // Try to load up the settings - it's ok if this fails, we'll just end up with the defaults
            let (settings, blacklist) = config::ConfigManager::load(handle.clone());
            app.state::<ManagedValue<Settings>>()
                .write(settings.clone())
                .ok();
            app.state::<ManagedValue<Blacklist>>().write(blacklist).ok();

            // Load extensions
            let parser_state = app
                .state::<ManagedValue<lavendeux_parser::ParserState>>()
                .inner();
            extension::ParserStateManager::reload(handle.clone(), parser_state).ok();

            // Run startup sequence
            if !settings.start_script.trim().is_empty() {
                parser::parse(settings.start_script, handle.clone());
            }

            // Set language
            TranslatorManager::set_language(
                settings.language_code.clone(),
                app.state::<ManagedTranslator>().inner(),
            );

            // Register the default shortcut
            settings.shortcut.register(app.handle()).ok();
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
            tauri::RunEvent::WindowEvent { event, .. } => match event {
                WindowEvent::CloseRequested { api, .. } => {
                    api.prevent_close();
                }
                _ => {}
            },
            tauri::RunEvent::Ready => {}
            tauri::RunEvent::MainEventsCleared => {}
            tauri::RunEvent::MenuEvent(_) => {}

            _ => {}
        }
    });
}
