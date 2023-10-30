import React, { useState } from "react";

import { TextboxSetting, SwitchSetting } from "./setting_container";
import { Translated } from "../../components";

import Form from 'react-bootstrap/Form';

interface Props {
    displayErrors: boolean,
    onChangeDisplayErrors: Function,
    
    darkMode: boolean,
    onChangeDarkMode: Function,
}

export const DisplaySettings = (props:Props) => {
    function renderDisplayErrors() {
        return (
            <SwitchSetting
                icon="bell-slash" title={<Translated path="settings\display\lbl_errors" />}
                desc={<Translated path="settings\display\desc_errors" />}
                id="display-errors" checked={props.displayErrors} 
                onChange={(e) => {
                    let elem = e.target as HTMLInputElement;
                    props.onChangeDisplayErrors(elem.checked);
                }}
            />
        );
    }

    function renderDarkMode() {
        return (
            <SwitchSetting
                icon="sun" title={<Translated path="settings\display\lbl_dark" />}
                desc={<Translated path="settings\display\desc_dark" />}
                id="display-errors" checked={props.darkMode} 
                onChange={(e) => {
                    let elem = e.target as HTMLInputElement;
                    props.onChangeDarkMode(elem.checked);
                }}
            />
        );
    }

    return (<>
        {renderDisplayErrors()}
        {renderDarkMode()}
    </>);
}