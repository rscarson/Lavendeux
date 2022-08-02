import { run, listen } from './tauri';

/**
 * Set the theme on an element
 * @param {DOM Object} element 
 * @param {bool} is_dark 
 */
export function setTheme(element, is_dark) {
	is_dark 
    ? element.classList.add('dark-theme')
    : element.classList.remove('dark-theme');
}

/**
 * Register a listener for theme changes
 * @param {DOM Object} element 
 */
export function registerThemeListener(element) {
	listen('settings', event => setTheme(element, event.payload.dark));
}

/**
 * Update the current theme
 * @param {DOM Object} element 
 */
export function updateTheme(element) {
    run("get_settings")
    .then(e => setTheme(element, e.dark))
    .catch(err => console.log(`Error updating theme: ${err}`));
}