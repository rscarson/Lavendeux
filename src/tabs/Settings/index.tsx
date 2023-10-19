import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/tauri";

import { Settings, KeyboardShortcut, Nullable } from '../../types';
import { Loadable, ShortcutRecorder } from '../../components';

import { ParserSettings } from "./parser";
import { DisplaySettings } from "./display";

import Card from 'react-bootstrap/Card';
import Nav from 'react-bootstrap/Nav';
import Form from 'react-bootstrap/Form';
import Row from "react-bootstrap/Row";
import Col from "react-bootstrap/Col";
import Tab from "react-bootstrap/Tab";
import Tabs from "react-bootstrap/Tabs";
import { GeneralSettings } from "./general";

interface Props {}

export const SettingsTab: React.FC<Props> = ({}) => {
    const [tab, setTab] = useState<string>("general");
    const [loaded, setLoaded] = useState<boolean>(false);

    const [enhancedClipboard, setEnhancedClipboard] = useState<boolean>(false);
    const [startScript, setStartScript] = useState<string>("");
    
    const [displayErrors, setDisplayErrors] = useState<boolean>(false);
    const [darkMode, setDarkMode] = useState<boolean>(false);

    const [keyboardShortcut, setKeyboardShortcut] = useState<KeyboardShortcut>({key:'', ctrl:false, alt: false, shift:false});
    const [startWithOs, setStartWithOs] = useState<boolean>(false);
    const [languageCode, setLanguageCode] = useState<string>("");

    async function load() {
        let settings: Nullable<Settings> = await invoke("read_settings", {});
        if (settings) {
            setEnhancedClipboard(settings.enhanced_clipboard);
            setDisplayErrors(settings.silence_errors);
            setStartScript(settings.start_script);
            setDarkMode(settings.dark_theme);
            setLanguageCode(settings.language_code);
            setKeyboardShortcut(settings.shortcut);
            setStartWithOs(settings.start_with_os);
            setLoaded(true);
        }
    }
    
    useEffect(() => {
        load();
    }, [])

    useEffect(() => {
        document.documentElement.dataset.bsTheme = darkMode?'dark':'';
    }, [darkMode])
    
    useEffect(() => {
        let settings: Settings = {
            enhanced_clipboard: enhancedClipboard,
            silence_errors: !displayErrors,
            start_script: startScript,
            dark_theme: darkMode,
            language_code: languageCode,
            shortcut: keyboardShortcut,
            start_with_os: startWithOs
        };
        invoke("write_settings", {settings});

    }, [enhancedClipboard, displayErrors, startScript, darkMode, languageCode, keyboardShortcut, startWithOs])

    return(<>
        <Row className="bg-body-secondary pt-2 row flex-fill flex-column">
            <Col className="m-2">settings</Col>
        </Row>
        <Row>
            <Tab.Container activeKey={tab}>
                <Col md={2} className="bg-body-secondary nav-sidebar">
                    <Nav className="flex-column" activeKey={tab} onSelect={(k) => k && setTab(k)}>
                        <Nav.Link eventKey="general" className="p-3">
                            <i className={"bi bi-gear"}>&nbsp;</i>
                            General
                        </Nav.Link>
                        <Nav.Link eventKey="parser" className="p-3">
                            <i className={"bi bi-file-earmark-code"}>&nbsp;</i>
                            Parsing
                        </Nav.Link>
                        <Nav.Link eventKey="display" className="p-3">
                            <i className={"bi bi-display"}>&nbsp;</i>
                            Display
                        </Nav.Link>
                    </Nav>
                </Col>
                <Col className="d-flex justify-content-center mb-4">
                    <Loadable loaded={loaded}>
                        <Tab.Content>
                            <Tab.Pane eventKey="general" title="General">
                                <GeneralSettings
                                    shortcut={keyboardShortcut} onChangeShortcut={(v) => setKeyboardShortcut(v)}
                                    startWithOs={startWithOs} onChangeStartWithOs={(v) => setStartWithOs(v)}
                                    languageCode={languageCode} onChangeLanguageCode={(v) => setLanguageCode(v)}
                                />
                            </Tab.Pane>
                            <Tab.Pane eventKey="parser" title="Parser">
                                <ParserSettings
                                    enhancedClipboard={enhancedClipboard} onChangeEnhancedClipboard={(v) => setEnhancedClipboard(v)}
                                    startScript={startScript} onChangeStartScript={(v) => setStartScript(v)}
                                />
                            </Tab.Pane>
                            <Tab.Pane eventKey="display" title="Display">
                                <DisplaySettings
                                    displayErrors={displayErrors} onChangeDisplayErrors={(v) => setDisplayErrors(v)}
                                    darkMode={darkMode} onChangeDarkMode={(v) => setDarkMode(v)}
                                />
                            </Tab.Pane>
                        </Tab.Content>
                    </Loadable>
                </Col>
            </Tab.Container>
        </Row>
    </>);
};