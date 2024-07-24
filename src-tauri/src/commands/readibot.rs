use crate::controllers::ReadibotController;
use tauri::AppHandle;

#[tauri::command]
pub fn is_ready(app: AppHandle) -> bool {
    ReadibotController(app).is_ready()
}
