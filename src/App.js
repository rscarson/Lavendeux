import React, {useState, useEffect} from 'react'
import {Switch, Route} from "react-router-dom";

import Help from './help';
import History from './history';
import Extensions from './extensions';
import Settings from './settings';
import './App.css'

import { Tabs, Tab, Alert } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

import { listen, emit } from '@tauri-apps/api/event'
import { invoke } from '@tauri-apps/api/tauri'

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

function App() {
	const [error, setError] = useState('');
	const [activeTab, setActiveTab] = useState('history');
	const [errorTitle, setErrorTitle] = useState('Could not parse expression');
	const [errorVariant, setErrorVariant] = useState('danger');
	const [settings, setSettings] = useState({});
	const [history, setHistory] = useState([]);
	const [extensions, setExtensions] = useState([]);

	useEffect(() => {
		emit("ready", "payload");

		listen('message', event => {
			setError(event.payload.msg);
			setErrorTitle(event.payload.title);
			setErrorVariant(event.payload.variant);
		});

		listen('settings', event => {
			setSettings(event.payload);
		});

		listen('extensions', event => {
			setExtensions(event.payload);
		});

		listen('history', event => {
			setHistory(event.payload.reverse());
		});

		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});
	}, []);

	return (
		<Switch>
			<Route path="/error"
					render={(_props) => (
						<Alert variant={errorVariant} 
							id="error-window"
							onClose={() => run("hide_errorwindow")}
							style={{display: error.length?'inherit':'none', cursor: "pointer"}} dismissible
						>
							<div onClick={() => run("show_history_tab")}>
								<Alert.Heading>{errorTitle}</Alert.Heading>
								<p>{error}</p>
							</div>
						</Alert>
					)}
			/>

			<Route path="*"
					render={(_props) => (
						<Tabs activeKey={activeTab} onSelect={k => setActiveTab(k)} id="main-nav" className="mb-3 main-nav fixed-top">
							<Tab className="nav-tab" eventKey="history" title="History">
								<History history={history} onClear={() => run("clear_history")} />
							</Tab>
				
							<Tab className="nav-tab" eventKey="extensions" title="Extensions">
								<Extensions onImport={importExtension} extensions={extensions} onReload={() => run("reload_extensions")} onDisable={disableExtension} onOpen={() => run("open_extensions_dir")} />
							</Tab>
				
							<Tab className="nav-tab" eventKey="settings" title="Settings">
								<Settings settings={settings} onSettingsUpdated={s => run("update_settings", {settings: s})} />
							</Tab>
				
							<Tab className="nav-tab" eventKey="help" title="Help">
								<Help />
							</Tab>
						</Tabs>
					)}
			/>
		</Switch>
	)
}

export default App