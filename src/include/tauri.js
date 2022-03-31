import { invoke } from '@tauri-apps/api/tauri';

export { listen, emit } from '@tauri-apps/api/event';
export { 
	writeText as writeClipboard, 
	readText as readClipboard
} from '@tauri-apps/api/clipboard';

export { 
	open as openDialog 
} from "@tauri-apps/api/dialog";

export async function run(f, payload) {
	if (payload) {
		return await invoke(f, payload).catch(e => alert(e));
	} else {
		return await invoke(f).catch(e => alert(e));
	}
}

export function importExtension(f) {
	if (f === null) return;
	invoke("import_extension", {srcPath: f})
	.then(() => {})
	.catch(e => alert(e));
}

export function disableExtension(f) {
	invoke("disable_extension", {srcPath: f})
	.then(() => {})
	.catch(e => alert(e));
}