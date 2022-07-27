import React, {useState, useEffect} from 'react';
import { Row, Col, Card, Button } from 'react-bootstrap';
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
			<Card>
				<Card.Body>
					<Card.Title>Clipboard Mode</Card.Title>
					<div className="form-check">
						<input className="form-check-input" type="radio" name="autoPaste" id="autopasteOn" 
							checked={settings.auto_paste} onChange={e => updateSettings("auto_paste", true)} />
						<label className="form-check-label" htmlFor="autopasteOn">
						The keyboard-shortcut will parse and replace highlighted text, bypassing the clipboard
						</label>
					</div>
		
					<div className="form-check">
						<input className="form-check-input" type="radio" name="autoPaste" id="autopasteOff" 
							checked={!settings.auto_paste} onChange={e => updateSettings("auto_paste", false)} />
						<label className="form-check-label" htmlFor="autopasteOff">
						The keyboard-shortcut will parse the clipboard's contents and replace it with the answer 
						</label>
					</div>

				</Card.Body>
			</Card>
		);
	}

	/**
	 * Render the error handling settings
	 * @returns Rendered data
	 */
	function renderErrorMode() {
		return (
			<Card>
				<Card.Body>
					<Card.Title>Error Handling Mode</Card.Title>
		
					<div className="form-check">
						<input className="form-check-input" type="radio" name="silentErrors" id="silentErrorsOff" 
							checked={!settings.silent_errors} onChange={e => updateSettings("silent_errors", false)} />
						<label className="form-check-label" htmlFor="silentErrorsOff">
						Errors will be logged, and appears as a popup message
						</label>
					</div>
		
					<div className="form-check">
						<input className="form-check-input" type="radio" name="silentErrors" id="silentErrorsOn" 
							checked={settings.silent_errors} onChange={e => updateSettings("silent_errors", true)} />
						<label className="form-check-label" htmlFor="silentErrorsOn">
						Errors will be logged, but will otherwise be silent
						</label>
					</div>
				</Card.Body>
			</Card>
		);
	}

	/**
	 * Render the shortcut settings
	 * @returns Rendered data
	 */
	function renderKeyboardShortcut() {
		return (
			<Card>
				<Card.Body>
					<Card.Title>Keyboard Shortcut</Card.Title>
					<Row>
						<Col sm="4" key={"mod"}>
							<div className="form-group">
								<select className="form-control" value={shortcutModifier}
									onChange={e => {setShortcutModifier(e.target.value); updateSettings("shortcut", `${e.target.value}+${shortcutKey}`)}}>

									{mod_values.map(v => (
										<option key={v.key} value={v.key}>{v.name}</option>
									))}
								</select>
							</div>
						</Col>
						<Col sm="4" key={"key"}>
							<div className="form-group">
								<select className="form-control" value={shortcutKey}
									onChange={e => {setShortcutKey(e.target.value); updateSettings("shortcut", `${shortcutModifier}+${e.target.value}`)}}>

									{keys.map(v => (
										<option key={v} value={v}>{v}</option>
									))}
								</select>
							</div>
						</Col>
					</Row>
				</Card.Body>
			</Card>
		);
	}

	function renderMisc() {
		return (
			<Card>
				<Card.Body>
				<Card.Title>Application Settings</Card.Title>
				
				<Row>
					<Col sm="8">
					<div className="form-group">
						<label>
							<input type="checkbox" autocomplete="off" checked={settings.autostart}
								onChange={e => updateSettings("autostart", e.currentTarget.checked)} /> Start Lavendeux Automatically
						</label><br/>
						<label>
							<input type="checkbox" autocomplete="off" checked={settings.dark}
								onChange={e => updateSettings("dark", e.currentTarget.checked)} /> Dark Theme
						</label>
					</div>
					</Col>
				</Row>
				</Card.Body>
			</Card>
		);
	}

	/**
	 * Render the extension settings
	 * @returns Rendered data
	 */
	function renderExtensionDir() {
		return (
			<Card>
				<Card.Body>
				<Card.Title>Extension directory</Card.Title>
				
				<Row>
					<Col sm="8">
					<div className="form-group">
						<input type="text" className="form-control" placeholder="Path to extensions" value={settings.extension_dir} 
							onChange={e => updateSettings("extension_dir", e.target.value)}
						/>
					</div>
					</Col>
				</Row>
				</Card.Body>
			</Card>
		);
	}

	/**
	 * Render the save button
	 * @returns Rendered data
	 */
	function renderSaveButton() {
		return (
			<div style={{paddingBottom: '15px'}}>
				<Button variant="primary" onClick={() => run("update_settings", {settings: settings})}>
					Save
				</Button>
			</div>
		);
	}

	return (
		<div className="nav-content">
			{renderSaveButton()}
			{renderClipboardMode()}
			{renderErrorMode()}
			{renderMisc()}
			{renderKeyboardShortcut()}
			{renderExtensionDir()}

			{renderSaveButton()}
		</div>
	);
}

export default Settings;