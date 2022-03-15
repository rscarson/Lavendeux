use tauri::{SystemTrayHandle, SystemTray, CustomMenuItem, SystemTrayMenu, SystemTrayMenuItem};

pub struct Tray {
    handle: SystemTrayHandle
}

impl Tray {
    pub fn new(tray_handle: SystemTrayHandle) -> Self {
        Self {
            handle: tray_handle
        }
    }

    pub fn update_menu(&self, recent_history: Vec<String>) {
        self.handle.set_menu(Self::get_menu(recent_history)).ok();
    }

    pub fn new_tray() -> SystemTray {
        SystemTray::new().with_menu(Self::get_menu([].to_vec()))
    }

    pub fn get_menu(recent_history: Vec<String>) -> SystemTrayMenu {
        let mut menu = SystemTrayMenu::new();
        if recent_history.len() == 0 {
            menu = menu.add_item(CustomMenuItem::new("no_history".to_string(), "No history to display").disabled());
        } else {
            let mut i = 0;
            for item in recent_history {
                menu = menu.add_item(CustomMenuItem::new(format!("{}", i), item));
                i += 1;
            }
        }
        menu = menu
            .add_native_item(SystemTrayMenuItem::Separator)
            .add_item(CustomMenuItem::new("settings".to_string(), "Settings"))
            .add_item(CustomMenuItem::new("exit".to_string(), "Exit"));

        return menu;
    }
}