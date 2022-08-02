import React, {useState, useEffect} from 'react';
import { Tab, Nav } from 'react-bootstrap';

import { listen, emit } from '../include/tauri';
import { registerThemeListener, updateTheme } from '../include/theme';

import Help from '../components/tabs/help';
import History from '../components/tabs/history';
import Extensions from '../components/tabs/extensions';
import Settings from '../components/tabs/settings';
import "./main.css"

/**
 * A tab on the main window
 */
class WindowTab {
    /**
     * 
     * @param {string} key 
     * @param {string} icon 
     * @param {string} name 
     * @param {Component} content 
     */
    constructor(key, icon, name, content) {
        this.key = key;
        this.name = name;
        this.icon = icon;
        this.content = content;
    }

    /**
     * Render the nav for a tab
     * @returns JSX
     */
    render_nav() {
        return (
            <Nav.Link href={`#${this.key}`} eventKey={this.key}>
                <i class={`bi bi-${this.icon}`}>&nbsp;</i>
                {this.name}
            </Nav.Link>
        );
    }

    /**
     * Render the content for a tab
     * @returns JSX
     */
    render_content() {
        return (
            <Tab.Pane eventKey={this.key} title={this.name}>
                <this.content />
            </Tab.Pane>
        );
    }
}

const historyTab = new WindowTab('history', 'clock', 'History', History);
const extensionsTab = new WindowTab('extensions', 'plus', 'Extensions', Extensions);
const settingsTab = new WindowTab('settings', 'gear', 'Settings', Settings);
const helpTab = new WindowTab('help', 'question-circle', 'Help', Help);

const defaultTab = historyTab;

/**
 * Main settings window
 * @returns JSX
 */
function MainWindow() {
	const [activeTab, setActiveTab] = useState(defaultTab.key);

    /**
     * Emitted when the window first loads
     */
	useEffect(() => {
		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});

        registerThemeListener(document.documentElement);
        updateTheme(document.documentElement);

		emit("ready", "payload");
	}, []);

	return (<>
        <Tab.Container defaultActiveKey={defaultTab.key}>
            <Nav className="me-auto main-nav-menu">
                {historyTab.render_nav()}
                {extensionsTab.render_nav()}
                {settingsTab.render_nav()}
                {helpTab.render_nav()}
            </Nav>
            <Tab.Content>
                {historyTab.render_content()}
                {extensionsTab.render_content()}
                {settingsTab.render_content()}
                {helpTab.render_content()}
            </Tab.Content>
        </Tab.Container>
    </>)
}

export default MainWindow