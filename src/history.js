import React, {useState, useEffect} from 'react';
import { Button, ListGroup, Form, Row, Col, InputGroup, ButtonToolbar } from 'react-bootstrap';
import { writeText } from '@tauri-apps/api/clipboard'

import "bootstrap-icons/font/bootstrap-icons.css";

import { invoke } from '@tauri-apps/api/tauri'
export async function run(f, payload) {
	if (payload) {
		return await invoke(f, payload).catch(e => alert(e));
	} else {
		return await invoke(f).catch(e => alert(e));
	}
}

const MAX_DISPLAY_LEN = 60;
function shortenString(input, max) {
    let _input = input.slice(0, max);
    if (input.length !== _input.length) {
        _input += "[...]";
    }
    return _input;
} 

const example = "x = 8 + 3e+2\nx = sqrt(16)\nx**3 @hex";
function History(props) {
	const [shortcutName, setShortcutName] = useState("Ctrl+Space");
	const [autoPaste, setAutoPaste] = useState(true);

    useEffect(() => {
        setAutoPaste(props.settings.auto_paste === true);
        run("format_shortcut")
        .then(s => setShortcutName(s));
    }, [props.settings]);

    return (
        <div className="nav-content">
            <ListGroup variant="flush">
                <ListGroup.Item>
                    <Button onClick={e => props.onClear()} variant="outline-secondary" size="sm">Clear history</Button>
                </ListGroup.Item>
            {props.history.length ? props.history.map(entry => (
                <ListGroup.Item>
                    <span style={{float: "right"}} className="text-muted">
                        <small>2022-10-03 03:41:03 PM</small>
                    </span>

                    <ButtonToolbar>
                        <InputGroup>
                            <InputGroup.Text id="btnGroupAddon">{shortenString(entry.expression, MAX_DISPLAY_LEN)}</InputGroup.Text>
                            <Button variant="outline-secondary" onClick={() => writeText(entry.expression)}>
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
                                <Button variant="outline-secondary" onClick={() => writeText(entry.result.Ok)}>
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
                )) : (
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
                )}
            </ListGroup>
        </div>
    );
}

export default History;