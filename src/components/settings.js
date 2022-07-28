import React, {useState, useEffect} from 'react';
import { Row, Col, Button, ListGroup, ButtonGroup, ToggleButton } from 'react-bootstrap';
import { listen, run } from '../include/tauri';

// Range of values for the shortcut key
const alpha_range = Array.from({ length: 26 }, (_, i) => String.fromCharCode('A'.charCodeAt(0) + i));
const keys = ["Space", ...alpha_range, "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"];

// Range of values for the shortcut modifier
const mod_values = [
	{"key":"CmdOrCtrl", "name":"CTRL"},
	{"key":"Shift", "name":"SHIFT"},
	{"key":"CmdOrCtrl+Shift", "name":"CTRL+SHIFT"}
];

/**
 * Render the settings tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Settings(props) {
	const [shortcutModifier, setShortcutModifier] = useState("CmdOrCtrl");
	const [shortcutKey, setShortcutKey] = useState("Space");
	const [settings, setSettings] = useState({});

	useEffect(() => {
		listen('settings', event => {
			if (!event.payload.shortcut) return;
			
			let shortcut = event.payload.shortcut.split("+");
			setShortcutModifier(shortcut[0]);
			setShortcutKey(shortcut[1]);

			setSettings(event.payload);
		});
		
        run("get_settings")
        .then(e => {setSettings(e)})
        .catch(err => console.log(`Error: ${err}`));
	}, []);

	function updateSettings(key, value) {
		const newSettings = {
			...settings,
			[key]: value
		};

		setSettings(newSettings);
	}

	/**
	 * Render the clipboard mode settings
	 * @returns Rendered data
	 */
	function renderClipboardMode() {
		return (
			<>
				<h5 className="setting-name">Clipboard Mode</h5>
				<p className="theme-p">
					<small><strong>
						When the keyboard-shortcut is pressed, determines where text is pulled from
					</strong></small>
				</p>
				<div className="form-check">
					<input className="form-check-input" type="radio" name="autoPaste" id="autopasteOn" 
						checked={settings.auto_paste} onChange={e => updateSettings("auto_paste", true)} />
					<label className="form-check-label setting-label" htmlFor="autopasteOn">
					Replace highlighted text, bypassing the clipboard
					</label>
				</div>

				<div className="form-check">
					<input className="form-check-input" type="radio" name="autoPaste" id="autopasteOff" 
						checked={!settings.auto_paste} onChange={e => updateSettings("auto_paste", false)} />
					<label className="form-check-label setting-label" htmlFor="autopasteOff">
					Replace clipboard contents
					</label>
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
				<h5 className="setting-name">Silent Errors</h5>
				<p className="setting-description theme-p">
					<small><strong>
						If a problem occurs parsing an expression, suppresses the popup message
					</strong></small>
				</p>
				<ButtonGroup>
					<ToggleButton size="sm" variant="outline-danger" id="silentErrorsOff" key="off" type="radio" name="silentErrors" 
						checked={!settings.silent_errors} onChange={e => updateSettings("silent_errors", false)}>Off</ToggleButton>
					<ToggleButton size="sm" variant="outline-success" id="silentErrorsOn" key="on" type="radio" name="silentErrors" 
						checked={settings.silent_errors} onChange={e => updateSettings("silent_errors", true)}>On</ToggleButton>
				</ButtonGroup>
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
				<h5 className="setting-name">Start Automatically</h5>
				<p className="setting-description theme-p">
					<small><strong>
						Opens lavendeux automatically when logging in to your computer
					</strong></small>
				</p>
				<ButtonGroup>
					<ToggleButton size="sm" variant="outline-danger" id="autostartOff" key="off" type="radio" name="autostart" 
						checked={!settings.autostart} onChange={e => updateSettings("autostart", false)}>Off</ToggleButton>
					<ToggleButton size="sm" variant="outline-success" id="autostartOn" key="on" type="radio" name="autostart" 
						checked={settings.autostart} onChange={e => updateSettings("autostart", true)}>On</ToggleButton>
				</ButtonGroup>
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
				<h5 className="setting-name">Theme</h5>
				<p className="setting-description theme-p">
					<small><strong>
						Visual style for the settings window
					</strong></small>
				</p>
				<div className="form-group">
					<label className="setting-label">
						<input type="radio" name="dark" autocomplete="off" checked={!settings.dark}
							onChange={e => updateSettings("dark", false)} /> Light
					</label><br/>
					<label className="setting-label">
						<input type="radio" name="dark" autocomplete="off" checked={settings.dark}
							onChange={e => updateSettings("dark", true)} /> Dark
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
				<h5 className="setting-name">Keyboard Shortcut</h5>
				<p className="setting-description theme-p">
					<small><strong>
						Keyboard shortcut
					</strong></small>
				</p>
				<Row>
					<Col className="form-group">
						<select className="form-control" value={shortcutModifier}
							onChange={e => {setShortcutModifier(e.target.value); updateSettings("shortcut", `${e.target.value}+${shortcutKey}`)}}>

							{mod_values.map(v => (
								<option key={v.key} value={v.key}>{v.name}</option>
							))}
						</select>
					</Col>
					<Col className="form-group">
						<select className="form-control" value={shortcutKey}
							onChange={e => {setShortcutKey(e.target.value); updateSettings("shortcut", `${shortcutModifier}+${e.target.value}`)}}>

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
				<h5 className="setting-name">Extension directory</h5>
				<p className="setting-description theme-p">
					<small><strong>
						Imported extensions will be copied to this directory
					</strong></small>
				</p>
				<div className="form-group">
					<input type="text" className="form-control" placeholder="Path to extensions" value={settings.extension_dir} 
						onChange={e => updateSettings("extension_dir", e.target.value)} />
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
			<Button variant="outline-success" size="sm"  onClick={() => run("update_settings", {settings: settings})}>
				Save Changes
			</Button>
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
                <ListGroup.Item>{renderExtensionDir()}</ListGroup.Item>
				
                <ListGroup.Item>{renderSaveButton()}</ListGroup.Item>
			</ListGroup>
		</div>
	);
}

export default Settings;