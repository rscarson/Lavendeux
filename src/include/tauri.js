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
	readText as readClipboard,
} from '@tauri-apps/api/clipboard';

/**
 * Open the file selection dialog
 */
export {
	open as openDialog,
} from '@tauri-apps/api/dialog';

/**
 * Get Software details
 */
export { getName } from '@tauri-apps/api/app';
export { getVersion } from '@tauri-apps/api/app';

/**
 * Run a tauri function
 * @param {string} f Function name
 * @param {Object} payload
 * @returns Promise
 */
export function run(f, payload) {
	if (payload) {
		return invoke(f, payload).catch(e => alert(`Error running ${f}: ${e.stack}`));
	}
	return invoke(f).catch(e => alert(`Error running ${f}: ${e.stack}`));
}
