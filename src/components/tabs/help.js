import React, { useEffect, useState } from 'react';
import { Form, ListGroup } from 'react-bootstrap';
import { getName, getVersion } from '@tauri-apps/api/app';

import { SampleHTML, ExampleSample } from '../../include/formatter';
import { run, listen } from '../../include/tauri';

import "./help.css"

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
    const [lang, setLang] = useState({});

	useEffect(() => {
		getName().then(s => setName(s));
		getVersion().then(s => setVersion(s));

		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
        
        run("lang_en").then(l => setLang(l));
	}, []);

	return (
		<div className="nav-content">
			<ListGroup variant="flush">
				<ListGroup.Item>
					<h4>{name} <small>v{version}</small></h4>
				</ListGroup.Item>
				<ListGroup.Item>
					<p className='theme-p'>
						{lang.helpview_help_1} (<kbd>{shortcutName}</kbd>) {lang.helpview_help_2}
					</p>
					<p className='theme-p'>
						{lang.helpview_help_3}
					</p>
					<p className='theme-p'>
						{autoPaste
						? lang.historyview_getting_started_highlight
						: lang.historyview_getting_started_copy
						}
						&nbsp;<kbd>{shortcutName}</kbd>
					</p>
					<Form.Control as="textarea" style={{ height: '100px' }} defaultValue={ExampleSample}></Form.Control>
				</ListGroup.Item>
				<ListGroup.Item>
					<div dangerouslySetInnerHTML={
						{__html: SampleHTML}
					}></div>
				</ListGroup.Item>
			</ListGroup>
		</div>
	);
}

export default Help;