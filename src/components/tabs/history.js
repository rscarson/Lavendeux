import React, {useState, useEffect} from 'react';
import { Button, ListGroup, Form, Row, Col, InputGroup, ButtonToolbar } from 'react-bootstrap';

import { IconButton } from '../icon_button';
import { ExampleSample } from '../../include/formatter';
import { writeClipboard, run, listen } from '../../include/tauri';

import "./history.css"

const MAX_DISPLAY_LEN = 60;

/**
 * Shorten a given string
 * @param {String} input Input string
 * @param {int} max Max string length
 * @returns Shortened string
 */
function shortenString(input, max) {
    let _input = input.slice(0, max);
    if (input.length !== _input.length) {
        _input += "[...]";
    }
    return _input;
} 

/**
 * Render the history tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function History(props) {
	const [history, setHistory] = useState([]);
	const [shortcutName, setShortcutName] = useState("Ctrl+Space");
	const [autoPaste, setAutoPaste] = useState(true);
    const [lang, setLang] = useState({});

	useEffect(() => {
		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
        
		listen('history', event => {
			setHistory(event.payload.reverse());
		});
        
        run("lang_en").then(l => setLang(l));
	}, []);

    /**
     * Render a history entry
     * @param {Object} entry Entry to render
     * @returns Rendered entry
     */
    function renderEntry(entry) {
        return (
            <ListGroup.Item>
                <span style={{float: "right"}} className="text-muted">
                    <small>{entry.timestamp}</small>
                </span>

                <ButtonToolbar>
                    <InputGroup>
                        <InputGroup.Text id="btnGroupAddon">{shortenString(entry.expression, MAX_DISPLAY_LEN)}</InputGroup.Text>
                        <IconButton variant="secondary" onClick={() => writeClipboard(entry.expression)} icon="clipboard" title="" />
                    </InputGroup>
                    &nbsp;
                    <Button className="arrow-button" variant="dark" disabled>
                        <i className="icon bi-arrow-right"></i>
                    </Button>
                    &nbsp;
                    {entry.result.Ok ? (
                        <InputGroup>
                            <InputGroup.Text id="btnGroupAddon">{entry.result.Ok}</InputGroup.Text>
                            <IconButton variant="secondary" onClick={() => writeClipboard(entry.result.Ok)} icon="clipboard" />
                        </InputGroup>
                    ) : (
                        <InputGroup>
                            <Button disabled variant="danger">{entry.result.Err}</Button>
                        </InputGroup>
                    )}
                </ButtonToolbar>
            </ListGroup.Item>
        );
    }

    /**
     * Render empty history
     * @returns Rendered history
     */
    function renderEmptyHistory() {
        return (
            <ListGroup.Item className="text-center">
                <strong>{lang.historyview_no_history}</strong>
                <p className="theme-p">
                    {autoPaste
                    ? lang.historyview_getting_started_highlight
                    : lang.historyview_getting_started_copy
                    }
                    &nbsp;<kbd>{shortcutName}</kbd>
                </p>
                <Row>
                    <Col sm="3"></Col>
                    <Col sm="6">
                        <Form.Control as="textarea" style={{ height: '100px' }} defaultValue={ExampleSample}></Form.Control>
                    </Col>
                    <Col sm="3"></Col>
                </Row>
            </ListGroup.Item>
        );
    }

    return (
        <div className="nav-content">
            <ListGroup variant="flush">
                <ListGroup.Item>
                    <IconButton variant="secondary" onClick={() => run("clear_history")} icon="clock-history" title={lang.historyview_btn_clear} />
                </ListGroup.Item>

                { history.length 
                    ? history.map(entry => renderEntry(entry)) 
                    : renderEmptyHistory()
                }
            </ListGroup>
        </div>
    );
}

export default History;