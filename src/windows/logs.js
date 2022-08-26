import React, { useState, useEffect } from 'react';
import { Table, ListGroup, Button } from 'react-bootstrap';

import { listen, run } from '../include/tauri';
import { registerThemeListener, updateTheme } from '../include/theme';

/**
 * Render the log viewer
 * @returns Component contents
 */
function LogsWindow() {
	const [logs, setLogs] = useState([]);
	const [lang, setLang] = useState({});

	/**
     * Emitted when the window first loads
     */
	useEffect(() => {
		listen('logs', event => {
			setLogs(event.payload);
		});

		run('get_logs')
			.then(e => setLogs(e))
			.catch(err => console.log(`Error: ${err}`));

		registerThemeListener(document.documentElement);
		updateTheme(document.documentElement);
		run('get_lang').then(l => setLang(l));
	}, []);

	/**
     * Render a log entry in the table
     * @param {Object} row
     * @returns JSX
     */
	function renderRow(row) {
		return (
			<tr key={row.timestamp}>
				<td>{row.timestamp}</td>
				<td>{row.level}</td>
				<td>{row.text}</td>
			</tr>
		);
	}

	/**
     * Render the default placeholder
     * @returns JSX
     */
	function renderEmptyLog() {
		return (
			<tr>
				<td colSpan={3} className="text-center">
					{lang.logview_empty}
				</td>
			</tr>
		);
	}

	/**
	 * Event handlers
	 */
	const clearLogs = () => run('clear_logs');
	const openLogsDir = () => run('open_logs_dir');

	return (
		<ListGroup variant="flush">
			<ListGroup.Item>
				<Button onClick={clearLogs} variant="outline-secondary" size="sm">
					<i className="bi bi-clock-history">&nbsp;</i>
					{lang.logview_btn_clear}
				</Button>
&nbsp;
				<Button variant="outline-secondary" size="sm" onClick={openLogsDir}>
					<i className="bi bi-folder2-open">&nbsp;</i>
					{lang.logview_btn_open}
				</Button>
			</ListGroup.Item>

			<Table className="w-100" striped hover>
				<thead>
					<tr>
						<th className="col-sm-2">{lang.logview_label_timestamp}</th>
						<th>{lang.logview_label_level}</th>
						<th>{lang.logview_label_event}</th>
					</tr>
				</thead>
				<tbody>
					{ logs.length
						? logs.map(entry => renderRow(entry))
						: renderEmptyLog()}
				</tbody>
			</Table>
		</ListGroup>
	);
}

export default LogsWindow;
