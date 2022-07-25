import React, {useState, useEffect} from 'react';
import { listen } from '../include/tauri';

import Help from './help';
import History from './history';
import Extensions from './extensions';
import Settings from './settings';
import '../App.css'

import { Tabs, Tab } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

function MainWindow() {
	const [activeTab, setActiveTab] = useState('history');

	useEffect(() => {
		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});
	}, []);

	return (
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
	)
}

export default MainWindow