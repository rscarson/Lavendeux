import React, {useState, useEffect} from 'react';
import { Row, Col, Card, Button } from 'react-bootstrap';

const mod_values = [
	{"key":"CmdOrCtrl", "name":"CTRL"},
	{"key":"Shift", "name":"SHIFT"},
	{"key":"CmdOrCtrl+Shift", "name":"CTRL+SHIFT"}
];
const alpha_range = Array.from({ length: 26 }, (_, i) => String.fromCharCode('A'.charCodeAt(0) + i));
const keys = ["Space", ...alpha_range, "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"];
function Settings(props) {
	const [shortcutModifier, setShortcutModifier] = useState("CmdOrCtrl");
	const [shortcutKey, setShortcutKey] = useState("Space");
	const [settings, setSettings] = useState(props.settings);

    useEffect(() => {
		setSettings(props.settings);
    }, [props.settings]);

	function updateSettings(key, value) {
		const newSettings = {
			...settings,
			[key]: value
		};

		setSettings(newSettings);
	}

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

	function renderSaveButton() {
		return (
			<div style={{paddingBottom: '15px'}}>
				<Button variant="primary" onClick={() => props.onSettingsUpdated(settings)}>
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
			{renderKeyboardShortcut()}
			{renderExtensionDir()}

			{renderSaveButton()}
		</div>
	);
}

export default Settings;