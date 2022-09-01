import React, { useState, useEffect } from 'react';
import { Tab, Nav } from 'react-bootstrap';

import { listen, emit, run } from '../include/tauri';
import { registerThemeListener, updateTheme } from '../include/theme';

import Help from '../components/tabs/help';
import History from '../components/tabs/history';
import Extensions from '../components/tabs/extensions';
import Settings from '../components/tabs/settings';

import './css/main.css';
import '../include/formatter/formatter.css';

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
	render_nav(lang) {
		return (
			<Nav.Link href={`#${this.key}`} eventKey={this.key}>
				<i className={`bi bi-${this.icon}`}>&nbsp;</i>
				{lang[this.name]}
			</Nav.Link>
		);
	}

	/**
     * Render the content for a tab
     * @returns JSX
     */
	render_content(lang) {
		return (
			<Tab.Pane eventKey={this.key} title={lang[this.name]}>
				<this.content lang={lang} />
			</Tab.Pane>
		);
	}
}

const historyTab = new WindowTab('history', 'clock', 'mainview_history', History);
const extensionsTab = new WindowTab('extensions', 'puzzle', 'mainview_extensions', Extensions);
const settingsTab = new WindowTab('settings', 'gear', 'mainview_settings', Settings);
const helpTab = new WindowTab('help', 'question-circle', 'mainview_help', Help);

const defaultTab = historyTab;

/**
 * Main settings window
 * @returns JSX
 */
function MainWindow() {
	const [activeTab, setActiveTab] = useState(defaultTab.key);
	const [lang, setLang] = useState({});

	/**
     * Emitted when the window first loads
     */
	useEffect(() => {
		listen('switch_tab', event => {
			setActiveTab(event.payload.toLowerCase());
		});

		registerThemeListener(document.documentElement);
		updateTheme(document.documentElement);

		emit('ready', 'payload');

		run('get_language_strings').then(l => setLang(l));
		listen('settings', () => {
			run('get_language_strings').then(l => setLang(l));
		});
	}, []);

	return (
		<Tab.Container
			activeKey={activeTab}
			defaultActiveKey={defaultTab.key}
			onSelect={setActiveTab}
		>
			<Nav className="me-auto main-nav-menu">
				{historyTab.render_nav(lang)}
				{extensionsTab.render_nav(lang)}
				{settingsTab.render_nav(lang)}
				{helpTab.render_nav(lang)}
			</Nav>
			<Tab.Content>
				{historyTab.render_content(lang)}
				{extensionsTab.render_content(lang)}
				{settingsTab.render_content(lang)}
				{helpTab.render_content(lang)}
			</Tab.Content>
		</Tab.Container>
	);
}

export default MainWindow;
