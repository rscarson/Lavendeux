import React, {useState, useEffect} from 'react';
import { Button, ListGroup, Form, Row, Col, InputGroup, ButtonToolbar } from 'react-bootstrap';
import { writeClipboard, run, listen } from '../include/tauri';

import "bootstrap-icons/font/bootstrap-icons.css";

const MAX_DISPLAY_LEN = 60;
const example = `
x = 8 + 3e+2
x = sqrt(16)
x**3 @hex
`.trim();

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

	useEffect(() => {
		listen('settings', event => {
            setAutoPaste(event.payload.auto_paste === true);
            run("format_shortcut")
                .then(s => setShortcutName(s));
		});
        
		listen('history', event => {
			setHistory(event.payload.reverse());
		});
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
                    <small>2022-10-03 03:41:03 PM</small>
                </span>

                <ButtonToolbar>
                    <InputGroup>
                        <InputGroup.Text id="btnGroupAddon">{shortenString(entry.expression, MAX_DISPLAY_LEN)}</InputGroup.Text>
                        <Button variant="outline-secondary" onClick={() => writeClipboard(entry.expression)}>
                            <i className="icon bi-clipboard"></i>
                        </Button>
                    </InputGroup>
                    &nbsp;
                    <Button className="arrow-button" variant="dark" disabled>
                        <i className="icon bi-arrow-right"></i>
                    </Button>
                    &nbsp;
                    {entry.result.Ok ? (
                        <InputGroup>
                            <InputGroup.Text id="btnGroupAddon">{entry.result.Ok}</InputGroup.Text>
                            <Button variant="outline-secondary" onClick={() => writeClipboard(entry.result.Ok)}>
                                <i className="icon bi-clipboard"></i>
                            </Button>
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
                <strong>No history to display.</strong>
                <p>To get started, try {autoPaste ? "highlighting" : "copying"} the following block of text, and pressing <kbd>{shortcutName}</kbd></p>
                <Row>
                    <Col sm="3"></Col>
                    <Col sm="6">
                        <Form.Control as="textarea" style={{ height: '100px' }} defaultValue={example}></Form.Control>
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
                    <Button onClick={e => run("clear_history")} variant="outline-secondary" size="sm">Clear history</Button>
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