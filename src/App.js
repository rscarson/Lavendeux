import React, {useState, useEffect} from 'react';
import {Routes, Route} from "react-router-dom";
import { emit, listen } from './include/tauri';

import Error from './components/error';
import Help from './components/help';
import History from './components/history';
import Extensions from './components/extensions';
import Settings from './components/settings';
import './App.css'

import { Tabs, Tab } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
	const [activeTab, setActiveTab] = useState('history');

	useEffect(() => {
		emit("ready", "payload");
		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});
	}, []);

	return (
		<Routes>
			<Route path="/error"
				element={
					<Error />
				}
			/>

			<Route path="*"
				element={
					<Tabs activeKey={activeTab} onSelect={k => setActiveTab(k)} id="main-nav" className="mb-3 main-nav fixed-top">
						<Tab className="nav-tab" eventKey="history" title="History">
							<History />
						</Tab>
			
						<Tab className="nav-tab" eventKey="extensions" title="Extensions">
							<Extensions />
						</Tab>
			
						<Tab className="nav-tab" eventKey="settings" title="Settings">
							<Settings />
						</Tab>
			
						<Tab className="nav-tab" eventKey="help" title="Help">
							<Help />
						</Tab>
					</Tabs>
				}
			/>
		</Routes>
	)
}

export default App