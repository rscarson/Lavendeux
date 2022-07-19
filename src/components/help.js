import React, { useEffect, useState } from 'react';
import { Form } from 'react-bootstrap';
import { Container } from 'react-bootstrap';
import { helpText } from '../include/help_strings';
import { run, listen } from '../include/tauri';

const example = `
x = 8 + 3e+2
x = sqrt(16)
x**3 @hex
`.trim();

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
		run("get_name").then(s => setName(s));
		run("get_version").then(s => setVersion(s));

		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
	}, []);

	return (
		<Container>
			<h4>{name} v{version}</h4>
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
			<Form.Control as="textarea" style={{ height: '100px' }} defaultValue={example}></Form.Control>
			
			<p className='theme-p'><hr /></p>
			<div dangerouslySetInnerHTML={
            	{__html: helpText()}
        	}></div>
		</Container>
	);
}

export default Help;