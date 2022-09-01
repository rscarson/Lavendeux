import React, { useState, useEffect } from 'react';
import { Alert } from 'react-bootstrap';

import { run, listen } from '../include/tauri';

import './css/error.css';

/**
 * Render the error model
 * @param {Object} props Component properties
 * @returns JSX
 */
function ErrorWindow() {
	const [error, setError] = useState('');
	const [errorTitle, setErrorTitle] = useState('');
	const [errorVariant, setErrorVariant] = useState('danger');

	/**
	 * Receive a new error from the host
	 * @param {Object} event Event details
	 */
	function errorEvent(event) {
		setError(event.payload.msg);
		setErrorTitle(event.payload.title);
		setErrorVariant(event.payload.variant);
	}

	/**
     * Emitted when the window first loads
     */
	useEffect(() => {
		listen('message', errorEvent);
	}, []);

	/**
	 * Event handlers
	 */
	const hideErrorWindow = () => run('hide_error');
	const showHistoryTab = () => run('show_history_tab');

	return (
		<Alert
			variant={errorVariant}
			id="error-window"
			onClose={hideErrorWindow}
			style={{ display: error.length ? 'inherit' : 'none', cursor: 'pointer' }}
			dismissible
		>
			<div onKeyDown={showHistoryTab} onClick={showHistoryTab} role="button" tabIndex={0}>
				<Alert.Heading>{errorTitle}</Alert.Heading>
				<p>{error}</p>
			</div>
		</Alert>
	);
}

export default ErrorWindow;
