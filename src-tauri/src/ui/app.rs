use std::sync::Mutex;
use std::path::Path;
use tauri::api::cli::{ Matches, get_matches };
use tauri::{
    AppHandle, Manager, WindowMenuEvent, 
    Menu, SystemTrayEvent, SystemTray, RunEvent,
    WindowEvent, Runtime, Builder, Invoke
};

use crate::core::{ SharedState, State, settings, extensions };
use crate::utils::{ fs, logs, autostart };
use crate::ui::{ windows, cmds, tray::Tray, keyboard };
use crate::parser;

struct AppBuilder<R: Runtime>(Builder<R>);
impl<R: Runtime> AppBuilder<R> {
    // Create a new instance of the builder
    pub fn new() -> Self {
        Self(tauri::Builder::<R>::new())
    }

    /// Manage the application's state
    /// 
    /// # Arguments
    /// * `state` - Application status
    pub fn manage_state(self, state: &State) -> Self {
        Self(
            self.0.manage( SharedState(Mutex::new(state.clone())) )
        )
    }

    /// Create the application tray
    /// 
    /// # Arguments
    /// * `tray` - Tray menu
    /// * `handler` - Event handler
    pub fn manage_tray<F>(self, tray: SystemTray, handler: F) -> Self
    where F: Fn(&AppHandle<R>, SystemTrayEvent) + Send + Sync + 'static {
        Self(
            self.0.system_tray(tray).on_system_tray_event(handler)
        )
    }

    /// Create the default window menu
    /// 
    /// # Arguments
    /// * `menu` - Window menu
    /// * `handler` - Event handler
    pub fn set_menu<F>(self, menu: Menu, handler: F) -> Self
    where F: Fn(WindowMenuEvent<R>) + Send + Sync + 'static {
        Self(
            self.0.menu(menu).on_menu_event(handler)
        )
    }

    /// Set the command handler
    /// 
    /// # Arguments
    /// * `handler` - Event handler
    pub fn set_handler<F>(self, handler: F) -> Self
    where F: Fn(Invoke<R>) + Send + Sync + 'static {
        Self(
            self.0.invoke_handler(handler)
        )
    }

    // Build the application
    pub fn build(self) -> tauri::App<R> {
        self.0.build(tauri::generate_context!())
              .expect("error while running tauri application")
    }
}

pub struct App(tauri::App);
impl App {
    /// Create a new instance of the app
    /// 
    /// # Arguments
    /// * `state` - Application status
    pub fn new(state: &State) -> Self {
        let builder = AppBuilder::<tauri::Wry>::new()
            .manage_state(state)
            .manage_tray(Tray::new_tray([].to_vec()), Tray::handle_event)
            .set_menu(windows::Main::get_menu(), windows::Main::handle_menu_event)
            .set_handler(tauri::generate_handler![
                cmds::update_settings, cmds::get_settings, cmds::format_shortcut,		    // Settings
                cmds::hide_error, cmds::clear_history, cmds::show_history_tab,	            // Windows
                cmds::get_logs, cmds::open_logs_dir, cmds::clear_logs,					    // Logs
                cmds::get_language_strings, cmds::get_language_string, cmds::set_language,  // Language
                cmds::import_extension, cmds::disable_extension, cmds::get_extensions,	    // Extensions
                cmds::reload_extensions, cmds::open_extensions_dir,
            ]);

        Self(builder.build())
    }

    /// Register the run handler
    pub fn run(self) {
        self.0.run(move |app_handle, e| match e {
            // Runs when the application starts
            RunEvent::Ready => {
                let shared_state: tauri::State<SharedState> = app_handle.state();

                // Read commandline arguments
                if let Ok(mut lock) = shared_state.0.lock() {
                    App::process_arguments(app_handle.clone(), &mut lock);
                    
                
                    // Apply default settings
                    let _settings = lock.settings.clone();
                    match App::update_settings(&app_handle, &mut lock, _settings) {
                        Ok(_) => {},
                        Err(e) => println!("Could not apply settings: {}", e)
                    };
        
                    // Await ready signal
                    App::signal_ready(app_handle.clone(), lock.settings.clone());
                };
            },
    
            // Prevent window closing
            RunEvent::WindowEvent { label, event, .. } => {
                App::hide_on_close_request(app_handle.clone(), &label, event);
            },
    
            // Keep the event loop running even if all windows are closed
            // This allow us to catch system tray events when there is no window
            RunEvent::ExitRequested { api, .. } => {
                api.prevent_exit();
            },
    
            _ => {}
        })
    }

    /// Signal readiness
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `settings` - App settings
    pub fn signal_ready(app_handle: AppHandle, settings: settings::Settings) {
        let _app_handle = app_handle.clone();
        app_handle.listen_global("ready", move |_| {
            // Prepare error window
            match windows::Error::new(_app_handle.clone()) {
                Some(w) => {
                    w.show_message(
                        "Ready to go!", 
                        &format!("Solve highlighted equations with {}", settings::shortcut_name(&settings.shortcut)), 
                        "success"
                    ).ok();
                },
                None => {}
            };
        });
    }

    /// Update application state in response to settings
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `state` - Application state
    /// * `settings` - App settings
    pub fn update_settings(app_handle: &AppHandle, state: &mut State, settings: settings::Settings) -> Result<settings::Settings, String> {
        let error_window = windows::Error::new(app_handle.clone()).ok_or("Error window does not exist")?;
		state.logger.debug("Updating settings");

        // Reload extensions
		extensions::reload(state)?;
        
        // Update keyboard shortcut
        if let Some(e) = keyboard::bind_shortcut(&app_handle, state, &settings.shortcut, parser::handle_shortcut) {
            state.logger.error(&e);
            error_window.show_message("Error saving keyboard shortcut", &e, "danger").ok();
            return Err(e);
        }
        
		// Create the extensions dir if needed
		if let Err(e) = std::fs::create_dir_all(
			Path::new(&(fs::compile_path(&[ settings.extension_dir.clone(), "disabled_extensions".to_string() ])?))
		) {
            let error = format!("Error creating the .lavendeux/extensions dir: {}", e);
			state.logger.error(&error);
			return Err(error);
		}

		// Update autostart
		if let Some(e) = autostart::update(settings.autostart) {
            let error = format!("Error updating autostart: {}", e);
			state.logger.error(&error);
			return Err(error);
		}

		// Update state
		state.settings = settings.clone();

		// Save settings to file
		if let Err(e) = settings.write() {
            let error = format!("Error saving settings: {}", e);
			state.logger.error(&error);
			return Err(error);
		}

        app_handle.emit_all("settings", settings.clone()).unwrap();
		state.logger.debug("Succesfully updated settings");
        Ok(settings)
    }

    /// Hide the main window instead of closing it
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `label` - window label
    /// * `event` - Event handler
    fn hide_on_close_request(app_handle: AppHandle, label: &str, event: WindowEvent) {
        match event {
            WindowEvent::CloseRequested { api, .. } => {
                if label == "main" {
                    let window = windows::Main::new(app_handle.clone()).unwrap();
                    window.hide().ok();
                    api.prevent_close();
                }
            },
            _ => {}
        };
    }

    /// Process a commandline argument
    /// 
    /// # Arguments
    /// * `argument` - Argument name
    /// * `matches` - Argument matches
    /// * `handler` - Event handler
    fn process_argument<F: FnMut(&str)>(argument: &str, matches: &Matches, mut handler: F) {
        let arg_match = matches.args.get(argument).unwrap();
        if arg_match.occurrences > 0 {
            handler(arg_match.value.as_str().unwrap());
        }
    }

    /// Process the commandline arguments
    /// 
    /// # Arguments
    /// * `app_handle` - Application handle
    /// * `state` - Application status
    fn process_arguments(app_handle: AppHandle, state: &mut State) {
        // Get arguments
        let matches = get_matches(
            app_handle.config().tauri.cli.as_ref().unwrap(), 
            app_handle.package_info()
        ).unwrap();
    
        // Configuration path
        App::process_argument("config", &matches, |filename| {
            state.settings.filename = filename.to_string();
            if let Ok(new_settings) = settings::Settings::read(filename) {
                state.settings = new_settings
            }
        });
    
        // Debug level
        App::process_argument("log-level", &matches, |level| {
            match level.to_lowercase().as_str() {
                "silly" => state.logger.set_level(logs::Level::Silly),
                "debug" => state.logger.set_level(logs::Level::Debug),
                "warning" => state.logger.set_level(logs::Level::Warning),
                "error" => state.logger.set_level(logs::Level::Error),
                "critical" => state.logger.set_level(logs::Level::Critical),
                _ => {}
            }
        });
    }
}