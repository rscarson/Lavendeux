// Prevents additional console window on Windows in release, DO NOT REMOVE!!
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::sync::{Mutex, MutexGuard};

mod models;
use models::settings;

pub struct ManagedValue<T>(Mutex<T>);
impl<T> ManagedValue<T> {
    pub fn new(value: T) -> Self {
        Self(Mutex::new(value))
    }

    pub fn read(&self) -> Option<MutexGuard<T>> {
        self.0.lock().ok()
    }

    pub fn write(&self, value: T) -> Result<(), ()> {
        if let Ok(mut lock) = self.0.lock() {
            *lock = value;
            Ok(())
        } else {
            Err(())
        }
    }
}

fn main() {
    tauri::Builder::default()
        .manage(ManagedValue::new(settings::Settings::default()))
        .plugin(tauri_plugin_window::init())
        .plugin(tauri_plugin_shell::init())
        .invoke_handler(tauri::generate_handler![
            settings::read_settings,
            settings::write_settings
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
