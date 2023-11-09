import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/primitives";
import { getCurrent } from "@tauri-apps/api/window";

import { Settings, KeyboardShortcut, Nullable } from '../../types';

import { RootTab } from "../Tab";
import { ParserSettings } from "./parser";
import { DisplaySettings } from "./display";
import { GeneralSettings } from "./general";

import Alert from 'react-bootstrap/Alert';
import Nav from 'react-bootstrap/Nav';
import Tab from "react-bootstrap/Tab";
import { Translated, clearTranslationCache } from "../../components";

interface Props {}

export const SettingsTab: React.FC<Props> = ({}) => {
    const [lastError, setLastError] = useState<string>("");
    useEffect(() => {
        if (!lastError) return;
        setTimeout(() => {
            setLastError("");
        }, 2000);
    }, [lastError]);

    const [tab, setTab] = useState<string>("general");
    const [loaded, setLoaded] = useState<boolean>(false);

    const [enhancedClipboard, setEnhancedClipboard] = useState<boolean>(false);
    const [startScript, setStartScript] = useState<string>("");
    
    const [displayErrors, setDisplayErrors] = useState<boolean>(false);
    const [darkMode, setDarkMode] = useState<boolean>(false);

    const [keyboardShortcut, setKeyboardShortcut] = useState<KeyboardShortcut>({key:'', ctrl:false, alt: false, shift:false});
    const [startWithOs, setStartWithOs] = useState<boolean>(false);
    const [languageCode, setLanguageCode] = useState<string>("");

    const [languages, setLanguages] = useState<Array<[string, string]>>([['English', 'en']])
    
    function loadFromSettings(settings: Settings) {
        if (enhancedClipboard != settings.enhanced_clipboard) setEnhancedClipboard(settings.enhanced_clipboard);
        if (displayErrors != settings.display_errors) setDisplayErrors(settings.display_errors);
        if (startScript != settings.start_script) setStartScript(settings.start_script);
        if (darkMode != settings.dark_theme) setDarkMode(settings.dark_theme);
        if (languageCode != settings.language_code) setLanguageCode(settings.language_code);
        if (keyboardShortcut != settings.shortcut) setKeyboardShortcut(settings.shortcut);
        if (startWithOs != settings.start_with_os) setStartWithOs(settings.start_with_os);
    }

    function writeSettings() {
        let settings: Settings = {
            enhanced_clipboard: enhancedClipboard,
            display_errors: displayErrors,
            start_script: startScript,
            dark_theme: darkMode,
            language_code: languageCode,
            shortcut: keyboardShortcut,
            start_with_os: startWithOs,
        };
        invoke("write_settings", {settings}).catch(e => {
            setLastError(`Error saving settings: ${e}`);
        });
    }

    async function load() {
        let settings: Nullable<Settings> = await invoke("read_settings", {});
        if (settings) {
            loadFromSettings(settings);
            setLoaded(true);
        }
    }
    
    useEffect(() => {
        load();

        const appWindow = getCurrent();
        appWindow.listen("updated-settings", (event) => loadFromSettings(event.payload as Settings))
    }, []);
    
    useEffect(() => {
        if (!loaded) return;
        writeSettings();
    }, [enhancedClipboard, displayErrors, startScript, darkMode, languageCode, keyboardShortcut, startWithOs])

    function renderSidebar() {
        return (
            <Nav className="flex-column" variant="pills" activeKey={tab} onSelect={(k) => k && setTab(k)}>
                <Nav.Link eventKey="general" className="p-3">
                    <i className={"bi bi-gear"}>&nbsp;</i>
                    <Translated path="settings\general\title" />
                </Nav.Link>

                <Nav.Link eventKey="parser" className="p-3">
                    <i className={"bi bi-file-earmark-code"}>&nbsp;</i>
                    <Translated path="settings\parsing\title" />
                </Nav.Link>
                
                <Nav.Link eventKey="display" className="p-3">
                    <i className={"bi bi-display"}>&nbsp;</i>
                    <Translated path="settings\display\title" />
                </Nav.Link>

                <hr />
                
                <Nav.Link className="p-3" onClick={() => invoke("open_config_dir", {})}>
                    <i className={"bi bi-folder2-open"}>&nbsp;</i>
                    <Translated path="settings\btn_browse" />
                </Nav.Link>

                <hr />
                
                <Nav.Link className="p-3" onClick={() => invoke("activate_debug", {})}>
                    <i className={"bi bi-bug"}>&nbsp;</i>
                    <Translated path="settings\btn_debug" />
                </Nav.Link>
                
                <Nav.Link className="p-3" onClick={() => invoke("app_exit", {})}>
                    <i className={"bi bi-x-octagon"}>&nbsp;</i>
                    <Translated path="settings\btn_quit" />
                </Nav.Link>
            </Nav>
        );
    }

    function renderContent() {
        return (<>
            <Alert show={!!lastError} className="fixed-top m-3" variant="danger" onClose={() => setLastError("")} dismissible>
                {lastError}
            </Alert>
            <Tab.Content>
                <Tab.Pane eventKey="general">
                    <GeneralSettings
                        shortcut={keyboardShortcut} onChangeShortcut={(v) => setKeyboardShortcut(v)}
                        startWithOs={startWithOs} onChangeStartWithOs={(v) => setStartWithOs(v)}
                        languageCode={languageCode} onChangeLanguageCode={(v) => {
                            setLanguageCode(v);
                            clearTranslationCache();
                        }}
                    />
                </Tab.Pane>
                <Tab.Pane eventKey="parser">
                    <ParserSettings
                        enhancedClipboard={enhancedClipboard} onChangeEnhancedClipboard={(v) => setEnhancedClipboard(v)}
                        startScript={startScript} onChangeStartScript={(v) => setStartScript(v)}
                    />
                </Tab.Pane>
                <Tab.Pane eventKey="display">
                    <DisplaySettings
                        displayErrors={displayErrors} onChangeDisplayErrors={(v) => setDisplayErrors(v)}
                        darkMode={darkMode} onChangeDarkMode={(v) => setDarkMode(v)}
                    />
                </Tab.Pane>
            </Tab.Content>
        </>);
    }

    return(
        <RootTab
            tab={tab} loaded={loaded}
            sidebar={renderSidebar()}
            content={renderContent()}
        />
    );
};