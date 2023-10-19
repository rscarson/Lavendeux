import React, { useState } from "react";

import { KeyboardShortcut } from "../../types";
import { DropdownSetting, KeyboardSetting, SwitchSetting } from "./setting_container";

import Form from 'react-bootstrap/Form';

interface Props {
    shortcut: KeyboardShortcut,
    onChangeShortcut: Function,
    
    startWithOs: boolean,
    onChangeStartWithOs: Function,
    
    languageCode: string,
    onChangeLanguageCode: Function,
}

export const GeneralSettings = (props:Props) => {
    function renderStartWithOs() {
        return (
            <SwitchSetting
                icon="windows" title="Start Lavendeux automatically"
                desc="When enabled, Starts Lavendeux automatically when you log in"
                id="display-errors" checked={props.startWithOs} 
                onChange={(e) => {
                    let elem = e.target as HTMLInputElement;
                    props.onChangeStartWithOs(elem.checked);
                }}
            />
        );
    }
    
    function renderKeyboardShortcut() {
        return (
            <KeyboardSetting
                icon="keyboard" title="Keyboard Shortcut"
                desc="The keyboard shortcut that will activate the parser. Defaults to CTRL-Space"
                value={props.shortcut} onChange={(s) => props.onChangeShortcut(s)}
            />
        );
    }

    function renderLanguageCode() {
        return (
            <DropdownSetting
                icon="translate" title="Language"
                desc="Language to use in the software. Parser errors will be displayed in english"
                value={props.languageCode} onChange={(e) => {
                    let elem = e.target as HTMLSelectElement;
                    props.onChangeLanguageCode(elem.value);
                }}
            />
        );
    }

    return (<>
        {renderKeyboardShortcut()}
        {renderStartWithOs()}
        {renderLanguageCode()}
    </>);
}