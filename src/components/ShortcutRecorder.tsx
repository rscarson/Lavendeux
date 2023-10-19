import React, { useState } from "react";

import { KeyboardShortcutController } from "../util/keyboard_shortcut";
import { KeyboardShortcut, Nullable } from "../types";

import Form from 'react-bootstrap/Form';
import OverlayTrigger from 'react-bootstrap/OverlayTrigger';
import Button from 'react-bootstrap/Button';
import Dropdown from 'react-bootstrap/Dropdown';
import Popover from 'react-bootstrap/Popover';
import InputGroup from 'react-bootstrap/InputGroup';
import SplitButton from 'react-bootstrap/SplitButton';
import Row from "react-bootstrap/Row";
import Col from "react-bootstrap/Col";

interface Props {
    shortcut: KeyboardShortcut,
    onShortcutChanged: Function
}

const popover = (
    <Popover id="popover-basic">
        <Popover.Header as="h3">Bind Keyboard Shortcut</Popover.Header>
        <Popover.Body>
            Press the key combination you'd like to use!<br/>
            The default option is CTRL+Space
        </Popover.Body>
    </Popover>
);

export const ShortcutRecorder = (props:Props) => {
    const [showPopover, setShowPopover] = useState<boolean>(false);

    return (<>
        <InputGroup size="sm">
            <OverlayTrigger show={showPopover} trigger="click" placement="top" overlay={popover} 
                onEnter={() => KeyboardShortcutController.record().then((key) => {
                    console.log(key);
                    setShowPopover(false);
                    props.onShortcutChanged(key as KeyboardShortcut);
                })}
                onExit={KeyboardShortcutController.stopRecording} 
            >
                <Button variant="danger" onClick={() => setShowPopover(!showPopover)}>Record</Button>
            </OverlayTrigger>
            <Form.Control
                disabled={true} value={KeyboardShortcutController.toString(props.shortcut)}
            />
        </InputGroup>
    </>);
}