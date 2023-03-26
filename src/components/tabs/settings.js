import React, { useState, useEffect } from 'react';
import { Row, Col, ListGroup } from 'react-bootstrap';

import { IconButton } from '../icon_button';
import { listen, run } from '../../include/tauri';

import './css/settings.css';

// Range of values for the shortcut key
const alphaRange = Array.from({ length: 26 }, (_, i) => String.fromCharCode('A'.charCodeAt(0) + i));
const keys = ['Space', ...alphaRange, 'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12', 'F13', 'F14', 'F15', 'F16', 'F17', 'F18', 'F19', 'F20', 'F21', 'F22', 'F23', 'F24'];

const languages = {
	en: 'English',
	fr: 'Francais',
};

// Range of values for the shortcut modifier
const modValues = [
	{ key: '', name: 'NONE' },
	{ key: 'CmdOrCtrl+', name: 'CTRL' },
	{ key: 'Shift+', name: 'SHIFT' },
	{ key: 'CmdOrCtrl+Shift+', name: 'CTRL+SHIFT' },
];

/**
 * Render the settings tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Settings(props) {
	const {
		lang = {},
	} = props;

	const [shortcutModifier, setShortcutModifier] = useState('CmdOrCtrl+');
	const [shortcutKey, setShortcutKey] = useState('Space');
	const [settings, _setSettings] = useState({
		auto_paste: false,
		silent_errors: false,
		autostart: false,
		dark: false,
		shortcut: 'CmdOrCtrl+Space',
		onstart: '',
		language: 'en',
		extension_dir: '',
	});

	function setSettings(payload) {
		// Update keys
		console.log(payload);
		const index = payload.shortcut.lastIndexOf('+');
		const modifier = index === -1 ? '' : payload.shortcut.slice(0, index + 1);
		const key = index === -1 ? payload.shortcut : payload.shortcut.slice(index + 1);
		setShortcutModifier(modifier);
		setShortcutKey(key);

		_setSettings(payload);
	}

	useEffect(() => {
		listen('settings', event => setSettings(event.payload));
		run('get_settings').then(e => setSettings(e));
	}, []);

	function updateSettings(key, value) {
		const newSettings = {
			...settings,
			[key]: value,
		};
		console.log(newSettings);
		setSettings(newSettings);
	}

	/**
	 * Render the clipboard mode settings
	 * @returns Rendered data
	 */
	function renderClipboardMode() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_clipboard_mode}</dt>
					<dd className="col-sm-9">{lang.settingview_clipboard_mode_desc}</dd>
				</dl>

				<div className="form-group">
					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="autoPaste"
							id="autopasteOn"
							checked={settings.auto_paste}
							onChange={e => updateSettings('auto_paste', e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="autopasteOn">
							{lang.settingview_clipboard_mode_auto}
						</label>
					</div>

					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="autoPaste"
							id="autopasteOff"
							checked={!settings.auto_paste}
							onChange={e => updateSettings('auto_paste', !e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="autopasteOff">
							{lang.settingview_clipboard_mode_off}
						</label>
					</div>
				</div>
			</>
		);
	}

	/**
	 * Render the error handling settings
	 * @returns Rendered data
	 */
	function renderErrorMode() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_silent_errors}</dt>
					<dd className="col-sm-9" style={{}}>{lang.settingview_silent_errors_desc}</dd>
				</dl>

				<div className="form-group">
					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="silent_errors"
							id="silent_errorsOff"
							checked={!settings.silent_errors}
							onChange={e => updateSettings('silent_errors', !e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="silent_errorsOff">
							{lang.settingview_silent_errors_off}
						</label>
					</div>

					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="silent_errors"
							id="silent_errorsOn"
							checked={settings.silent_errors}
							onChange={e => updateSettings('silent_errors', e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="silent_errorsOn">
							{lang.settingview_silent_errors_on}
						</label>
					</div>
				</div>
			</>
		);
	}

	/**
	 * Render the autostart settings
	 * @returns Rendered data
	 */
	function renderAutostart() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_autostart}</dt>
					<dd className="col-sm-9">{lang.settingview_autostart_desc}</dd>
				</dl>

				<div className="form-group">
					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="autostart"
							id="autostartOff"
							checked={!settings.autostart}
							onChange={e => updateSettings('autostart', !e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="autostartOff">
							{lang.settingview_autostart_off}
						</label>
					</div>

					<div className="form-check">
						<input
							className="form-check-input"
							type="radio"
							name="autostart"
							id="autostartOn"
							checked={settings.autostart}
							onChange={e => updateSettings('autostart', e.target.checked)}
						/>
						<label className="form-check-label setting-label" htmlFor="autostartOn">
							{lang.settingview_autostart_on}
						</label>
					</div>
				</div>
			</>
		);
	}

	/**
	 * Render the theme settings
	 * @returns Rendered data
	 */
	function renderTheme() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_theme}</dt>
					<dd className="col-sm-9">{lang.settingview_theme_desc}</dd>
				</dl>

				<div className="form-group">
					<label className="setting-label" htmlFor="darkOff">
						<input
							type="radio"
							name="dark"
							id="darkOff"
							autoComplete="off"
							checked={!settings.dark}
							onChange={e => updateSettings('dark', !e.target.checked)}
						/>
						{' '}
						{lang.settingview_theme_off}
					</label>
					<br />
					<label className="setting-label" htmlFor="darkOn">
						<input
							type="radio"
							name="dark"
							id="darkOn"
							autoComplete="off"
							checked={settings.dark}
							onChange={e => updateSettings('dark', e.target.checked)}
						/>
						{' '}
						{lang.settingview_theme_on}
					</label>
				</div>
			</>
		);
	}

	/**
	 * Render the shortcut settings
	 * @returns Rendered data
	 */
	function renderKeyboardShortcut() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_keyboard_shortcut}</dt>
					<dd className="col-sm-9">{lang.settingview_keyboard_shortcut_desc}</dd>
				</dl>

				<Row>
					<Col className="form-group">
						<select
							className="form-control"
							id="shortcut_mod"
							value={shortcutModifier}
							onChange={e => { updateSettings('shortcut', `${e.target.value}${shortcutKey}`); }}
						>

							{modValues.map(v => (
								<option key={v.key} value={v.key}>{v.name}</option>
							))}
						</select>
					</Col>
					<Col className="form-group">
						<select
							className="form-control"
							id="shortcut_key"
							value={shortcutKey}
							onChange={e => { updateSettings('shortcut', `${shortcutModifier}${e.target.value}`); }}
						>

							{keys.map(v => (
								<option key={v} value={v}>{v}</option>
							))}
						</select>
					</Col>
				</Row>
			</>
		);
	}

	/**
	 * Render the extension settings
	 * @returns Rendered data
	 */
	function renderExtensionDir() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_extension_dir}</dt>
					<dd className="col-sm-9">{lang.settingview_extension_dir_desc}</dd>
				</dl>

				<div className="form-group">
					<input
						type="text"
						id="extension_dir"
						className="form-control"
						placeholder="Path to extensions"
						value={settings.extension_dir}
						onChange={e => updateSettings('extension_dir', e.target.value)}
					/>
				</div>
			</>
		);
	}

	/**
	 * Render the startup script  settings
	 * @returns Rendered data
	 */
	function renderStartupScript() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_startup_script}</dt>
					<dd className="col-sm-9">{lang.settingview_startup_script_desc}</dd>
				</dl>

				<div className="form-group">
					<textarea
						className="form-control"
						id="startup_script"
						rows="5"
						value={settings.onstart}
						onChange={e => updateSettings('onstart', e.target.value)}
					/>
				</div>
			</>
		);
	}

	/**
	 * Render the extension settings
	 * @returns Rendered data
	 */
	function renderLanguage() {
		return (
			<>
				<dl className="row row-setting">
					<dt className="col-sm-3">{lang.settingview_language}</dt>
					<dd className="col-sm-9">{lang.settingview_language_desc}</dd>
				</dl>

				<div className="form-group">
					<select
						value={settings.language}
						type="select"
						className="form-control form-select"
						onChange={e => updateSettings('language', e.target.value)}
					>
						{Object.keys(languages).map(l => (
							<option key={l} value={l}>{languages[l]}</option>
						))}
					</select>
				</div>
			</>
		);
	}

	/**
	 * Render the save button
	 * @returns Rendered data
	 */
	function renderSaveButton() {
		return (
			<IconButton variant="success" onClick={() => run('update_settings', { settings })} icon="save" title={lang.settingview_save} />
		);
	}

	return (
		<div className="nav-content">
			<ListGroup variant="flush">
				<ListGroup.Item>{renderSaveButton()}</ListGroup.Item>

				<ListGroup.Item>{renderClipboardMode()}</ListGroup.Item>
				<ListGroup.Item>{renderErrorMode()}</ListGroup.Item>
				<ListGroup.Item>{renderKeyboardShortcut()}</ListGroup.Item>
				<ListGroup.Item>{renderAutostart()}</ListGroup.Item>
				<ListGroup.Item>{renderTheme()}</ListGroup.Item>
				<ListGroup.Item>{renderLanguage()}</ListGroup.Item>
				<ListGroup.Item>{renderExtensionDir()}</ListGroup.Item>
				<ListGroup.Item>{renderStartupScript()}</ListGroup.Item>

				<ListGroup.Item>{renderSaveButton()}</ListGroup.Item>
			</ListGroup>
		</div>
	);
}

export default Settings;
