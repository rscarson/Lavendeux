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

	useEffect(() => {
		getName().then(s => setName(s));
		getVersion().then(s => setVersion(s));

		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
	}, []);

	return (
		<div className="nav-content">
			<ListGroup variant="flush">
				<ListGroup.Item>
					<h4>{name} <small>v{version}</small></h4>
				</ListGroup.Item>
				<ListGroup.Item>
					<p className='theme-p'>
						In Windows, by default the keyboard shortcut (<kbd>{shortcutName}</kbd>) will interpret any currently highlighted text as an expression, 
						parse that expression, and replace it with the resulting value.
					</p>
					<p className='theme-p'>
						On other platforms, or when the clipboard mode has been changed, then instead of using highlighted text, {name} will use the contents of the clipboard.
					</p>
					<p className='theme-p'>
						Try {autoPaste ? "highlighting" : "copying"} the following block of text, and pressing <kbd>{shortcutName}</kbd>
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