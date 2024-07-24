import React, { useState, useEffect } from "react";

import { KeyboardShortcut } from "../../types";
import { DropdownSetting, KeyboardSetting, SwitchSetting } from "./setting_container";

import { Translated } from "../../components";

interface Props {
    shortcut: KeyboardShortcut,
    onChangeShortcut: Function,
    
    startWithOs: boolean,
    onChangeStartWithOs: Function,
    
    languages: Array<[string, string]>,
    languageCode: string,
    onChangeLanguageCode: Function,
}

export const GeneralSettings = (props:Props) => {
    function renderStartWithOs() {
        return (
            <SwitchSetting
                icon="windows" title={<Translated path="settings\general\lbl_autostart" />}
                desc={<Translated path="settings\general\desc_autostart" />}
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
                icon="keyboard" title={<Translated path="settings\general\lbl_shortcut" />}
                desc={<Translated path="settings\general\desc_shortcut" />}
                value={props.shortcut} onChange={(s) => props.onChangeShortcut(s)}
            />
        );
    }

    function renderLanguageCode() {
        return (
            <DropdownSetting
                icon="translate" title={<Translated path="settings\general\lbl_lang" />}
                desc={<>
                <Translated path="settings\general\desc_lang" /><br/>
                <small className="text-muted">
                    [ <Translated path="settings\general\lbl_lang_disclaimer" /> ]
                </small>
                </>}
                values={props.languages.map((l) => {
                    let [name, code] = l;
                    return <option key={code} value={code}>{name}</option>
                })}
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