import { invoke } from '@tauri-apps/api/tauri';

/**
 * Event handling functions
 */
export { listen, emit } from '@tauri-apps/api/event';

/**
 * Clipboard IO operations
 */
export { 
	writeText as writeClipboard, 
	readText as readClipboard
} from '@tauri-apps/api/clipboard';

/**
 * Open the file selection dialog
 */
export { 
	open as openDialog 
} from "@tauri-apps/api/dialog";

/**
 * Run a tauri function
 * @param {string} f Function name
 * @param {Object} payload 
 * @returns Promise
 */
export async function run(f, payload) {
	if (payload) {
		return await invoke(f, payload).catch(e => alert(e));
	} else {
		return await invoke(f).catch(e => alert(e));
	}
}