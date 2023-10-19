import React, { ChangeEventHandler, PropsWithChildren } from "react";
import { KeyboardShortcut } from "../../types";
import { ShortcutRecorder } from "../../components";

import Card from 'react-bootstrap/Card';
import Row from "react-bootstrap/Row";
import Col from "react-bootstrap/Col";
import Form from "react-bootstrap/Form";

interface SettingContainerProps {
    icon: string
}

export const SettingContainer = (props: PropsWithChildren<SettingContainerProps>) => {
    return (
        <div className="mt-4 d-flex align-items-center justify-content-center w-100" style={{maxWidth:"700px"}}>
            <Card className="w-100">
                <Card.Body className="row">
                    <Col md="2">
                        <i className={`display-6 bi bi-${props.icon}`}></i>
                    </Col>
                    {props.children}
                </Card.Body>
            </Card>
        </div>
    );
}

interface SwitchSettingProps {
    icon: string,
    desc: string,
    title: string

    id: string,
    checked: boolean,
    onChange: ChangeEventHandler<HTMLInputElement>
}
export const SwitchSetting: React.FC<SwitchSettingProps> = (props) => {
    return (
        <SettingContainer icon={props.icon}>
            <Col>
                <strong className="d-block">{props.title}</strong>
                <small>{props.desc}</small>
            </Col>
            <Col sm="2" className="d-flex flex-column justify-content-center" style={{textAlign:"right"}}>
                <Form.Check
                    type="switch"
                    id={props.id}
                    checked={props.checked} 
                    onChange={props.onChange}
                />
            </Col>
        </SettingContainer>
    );
}

interface KeyboardSettingProps {
    icon: string,
    desc: string,
    title: string

    value: KeyboardShortcut,
    onChange: ChangeEventHandler<HTMLInputElement>
}
export const KeyboardSetting: React.FC<KeyboardSettingProps> = (props) => {
    return (
        <SettingContainer icon={props.icon}>
            <Col>
                <strong className="d-block">{props.title}</strong>
                <small>{props.desc}</small>
            </Col>
            <Row className="mt-2">
                <ShortcutRecorder 
                    shortcut={props.value} 
                    onShortcutChanged={(shortcut) => props.onChange(shortcut)}
                />
            </Row>
        </SettingContainer>
    );
}

interface DropdownSettingProps {
    icon: string,
    desc: string,
    title: string

    value: string,
    onChange: ChangeEventHandler<HTMLSelectElement>
}
export const DropdownSetting: React.FC<DropdownSettingProps> = (props) => {
    return (
        <SettingContainer icon={props.icon}>
            <Col>
                <strong className="d-block">{props.title}</strong>
                <small>{props.desc}</small>
            </Col>
            <Row className="mt-2">
                <Form.Select onChange={props.onChange}>
                    <option value="en">English</option>
                    <option value="fr">French</option>
                </Form.Select>
            </Row>
        </SettingContainer>
    );
}

interface TextboxSettingProps {
    icon: string,
    desc: string,
    title: string

    id: string,
    value: string,
    onChange: ChangeEventHandler<HTMLInputElement>
}
export const TextboxSetting: React.FC<TextboxSettingProps> = (props) => {
    return (
        <SettingContainer icon={props.icon}>
            <Col>
                <strong className="d-block">{props.title}</strong>
                <small>{props.desc}</small>
            </Col>
            <Row className="mt-2">
                <pre>
                    <Form.Control
                        as="textarea"
                        spellCheck={false}
                        rows={5}
                        id={props.id}
                        value={props.value} 
                        onChange={props.onChange}
                    />
                </pre>
            </Row>
        </SettingContainer>
    );
}