import React, {useState, useEffect} from 'react';
import { listen, emit } from '../include/tauri';

import Help from './help';
import History from './history';
import Extensions from './extensions';
import Settings from './settings';
import '../App.css'

import { Tab, Nav } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

function MainWindow() {
	const [activeTab, setActiveTab] = useState('history');

	useEffect(() => {
		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});

		emit("ready", "payload");
	}, []);

	return (<>
        <Tab.Container id="left-tabs-example" defaultActiveKey='history'>
            <Nav className="me-auto main-nav-menu">
                <Nav.Link href="#history" eventKey="history">
                    <i class="bi bi-clock">&nbsp;</i>
                    History
                </Nav.Link>
                <Nav.Link href="#extensions" eventKey="extensions">
                    <i class="bi bi-plus">&nbsp;</i>
                    Extensions
                </Nav.Link>
                <Nav.Link href="#settings" eventKey="settings">
                    <i class="bi bi-gear">&nbsp;</i>
                    Settings
                </Nav.Link>
                <Nav.Link href="#help" eventKey="help">
                    <i class="bi bi-question-circle">&nbsp;</i>
                    Help
                </Nav.Link>
            </Nav>
            <Tab.Content>
                <br />
                <Tab.Pane eventKey="history" title="History">
                    <History />
                </Tab.Pane>

                <Tab.Pane eventKey="extensions" title="Extensions">
                    <Extensions />
                </Tab.Pane>

                <Tab.Pane eventKey="settings" title="Settings">
                    <Settings />
                </Tab.Pane>

                <Tab.Pane eventKey="help" title="Help">
                    <Help />
                </Tab.Pane>
            </Tab.Content>
        </Tab.Container>
    </>)
}

export default MainWindow