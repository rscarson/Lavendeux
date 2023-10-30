import React, { useState } from "react";

import { KeyboardShortcutController } from "../util/keyboard_shortcut";
import { Translated } from "./Translated";
import { KeyboardShortcut } from "../types";

import Form from 'react-bootstrap/Form';
import OverlayTrigger from 'react-bootstrap/OverlayTrigger';
import Button from 'react-bootstrap/Button';
import Popover from 'react-bootstrap/Popover';
import InputGroup from 'react-bootstrap/InputGroup';

interface Props {
    shortcut: KeyboardShortcut,
    onShortcutChanged: Function
}

export const ShortcutRecorder = (props:Props) => {
    const [showPopover, setShowPopover] = useState<boolean>(false);

    const popover = (
        <Popover id="popover-basic">
            <Popover.Header as="h3">
                <Translated path="general\components\shortcut\lbl_key_bind_title" />
                </Popover.Header>
            <Popover.Body>
                <Translated path="general\components\shortcut\lbl_key_press" /><br/>
                <Translated path="general\components\shortcut\lbl_key_press_default" />
            </Popover.Body>
        </Popover>
    );

    return (<>
        <InputGroup size="sm" onBlur={() => setShowPopover(false)}>
            <OverlayTrigger show={showPopover} trigger="click" placement="top" overlay={popover} 
                onEnter={() => KeyboardShortcutController.record().then((key) => {
                    if (key == props.shortcut || !key.key.length) return;
                    setShowPopover(false);
                    props.onShortcutChanged(key as KeyboardShortcut);
                })}
                onExit={KeyboardShortcutController.stopRecording} 
            >
                <Button variant="danger" onClick={() => setShowPopover(!showPopover)}>
                    <Translated path="general\components\shortcut\btn_record" />
                </Button>
            </OverlayTrigger>
            <Form.Control
                disabled={true} value={KeyboardShortcutController.toString(props.shortcut)}
            />
        </InputGroup>
    </>);
}