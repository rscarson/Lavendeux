import React, { useState, useEffect } from 'react';
import { Alert } from 'react-bootstrap';

import { run, listen } from '../include/tauri';

import './error.css';

/**
 * Render the error model
 * @param {Object} props Component properties
 * @returns JSX
 */
function ErrorWindow(props) {
	const [error, setError] = useState('');
	const [errorTitle, setErrorTitle] = useState('');
	const [errorVariant, setErrorVariant] = useState('danger');
    const [lang, setLang] = useState({});

    /**
     * Emitted when the window first loads
     */
	useEffect(() => {
		listen('message', errorEvent);
        run("lang_en").then(l => {
			setLang(l);
			setErrorTitle(l.errorview_parsing_error);
		});
	}, []);

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
	 * Hide the error modal
	 */
	function hideErrorWindow() { run("hide_errorwindow"); }

	/**
	 * Open the settings window to the history tab
	 */
	function showHistoryTab() { run("show_history_tab"); }

	return (
        <Alert variant={errorVariant} 
            id="error-window" onClose={hideErrorWindow}
            style={{display: error.length?'inherit':'none', cursor: "pointer"}} dismissible
        >
            <div onClick={showHistoryTab}>
                <Alert.Heading>{errorTitle}</Alert.Heading>
                <p>{error}</p>
            </div>
        </Alert>
	);
}

export default ErrorWindow;