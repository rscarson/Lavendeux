import React, { useEffect, useState } from 'react';
import { Form, ListGroup } from 'react-bootstrap';
import Accordion from 'react-bootstrap/Accordion';

import { getExampleSample, getAllSamples } from '../../include/formatter';
import { run, listen, getName, getVersion } from '../../include/tauri';

import './css/help.css';

/**
 * Render the help tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Help(props) {
	const {
		lang = {},
	} = props;

	const donateLink = 'https://www.paypal.com/donate/?business=UXKPLM89RVU5L&no_recurring=0&item_name=Thank+you+for+supporting+open-source+software%21&currency_code=CAD';

	const [name, setName] = useState('[...]');
	const [version, setVersion] = useState('[...]');
	const [shortcutName, setShortcutName] = useState('Ctrl+Space');
	const [autoPaste, setAutoPaste] = useState(true);
	const [codeSamples, setCodeSamples] = useState([]);
	const [example, setExample] = useState('');

	useEffect(() => {
		setCodeSamples(getAllSamples());
		setExample(getExampleSample());

		getName().then(s => setName(s));
		getVersion().then(s => setVersion(s));

		listen('settings', event => {
			setAutoPaste(event.payload.auto_paste === true);
			run('format_shortcut')
				.then(s => setShortcutName(s));
		});
	}, []);

	function renderTitle() {
		return (
			<h4>
				<img src={`${process.env.PUBLIC_URL}/logo.png`} alt={name} />
				{' '}
				<small>
					v
					{version}
				</small>
			</h4>
		);
	}

	function renderPreamble() {
		return (
			<>
				<p className="theme-p">
					{lang.helpview_help_1}
					{' '}
					(
					<kbd>{shortcutName}</kbd>
					)
					{lang.helpview_help_2}
				</p>
				<p className="theme-p">
					{lang.helpview_help_3}
				</p>
				<p className="theme-p">
					{autoPaste
						? lang.historyview_getting_started_highlight
						: lang.historyview_getting_started_copy}
				&nbsp;
					<kbd>{shortcutName}</kbd>
				</p>
				<Form.Control as="textarea" style={{ height: '100px' }} defaultValue={example} />
			</>
		);
	}

	function renderSample(sample, i) {
		return (
			<Accordion.Item eventKey={i}>
				<Accordion.Header>{sample.shortDescription}</Accordion.Header>
				<Accordion.Body dangerouslySetInnerHTML={{ __html: sample.html }} />
			</Accordion.Item>
		);
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
					<Accordion defaultActiveKey="0">
						{codeSamples.map((sample, i) => renderSample(sample, i))}
					</Accordion>
				</ListGroup.Item>
				<ListGroup.Item>
					<a href={donateLink} target="_blank" rel="noreferrer">
						<i className="bi bi-cup-hot">
							&nbsp;Support Lavendeux by buying me a coffee
						</i>
					</a>
				</ListGroup.Item>
			</ListGroup>
		</div>
	);
}

export default Help;
