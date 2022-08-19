import React, { useEffect, useState } from 'react';
import { Form, ListGroup } from 'react-bootstrap';

import { SampleHTML, ExampleSample } from '../../include/formatter';
import { run, listen, getName, getVersion } from '../../include/tauri';

import "./css/help.css"

/**
 * Render the help tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Help(props) {
	const [name, setName] = useState('[...]');
	const [version, setVersion] = useState('[...]');
	const [shortcutName, setShortcutName] = useState("Ctrl+Space");
	const [autoPaste, setAutoPaste] = useState(true);

	useEffect(() => {
		getName().then(s => setName(s));
		getVersion().then(s => setVersion(s));

		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
	}, []);

	function renderTitle() {
		return (<>
			<h4>{name} <small>v{version}</small></h4>
		</>);
	}

	function renderPreamble() {
		return (<>
			<p className='theme-p'>
				{props.lang.helpview_help_1} (<kbd>{shortcutName}</kbd>) {props.lang.helpview_help_2}
			</p>
			<p className='theme-p'>
				{props.lang.helpview_help_3}
			</p>
			<p className='theme-p'>
				{autoPaste
				? props.lang.historyview_getting_started_highlight
				: props.lang.historyview_getting_started_copy
				}
				&nbsp;<kbd>{shortcutName}</kbd>
			</p>
			<Form.Control as="textarea" style={{ height: '100px' }} defaultValue={ExampleSample}></Form.Control>
		</>);
	}

	function renderSamples() {
		return (<>
			<div dangerouslySetInnerHTML={
				{__html: SampleHTML}
			}></div>
		</>);
	}

	return (
		<div className="nav-content">
			<ListGroup variant="flush">
				<ListGroup.Item>
					{renderTitle()}
				</ListGroup.Item>
				<ListGroup.Item>
					{renderPreamble()}
				</ListGroup.Item>
				<ListGroup.Item>
					{renderSamples()}
				</ListGroup.Item>
			</ListGroup>
		</div>
	);
}

export default Help;