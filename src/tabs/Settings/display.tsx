import React, { useState } from "react";
import { TextboxSetting, SwitchSetting } from "./setting_container";

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
                icon="bell-slash" title="Display parsing errors"
                desc="When enabled, parsing errors will appear in the corner of your screen as a small popup. Errors can still be viewed in the log if turned off"
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
                icon="sun" title="Dark mode"
                desc="When enabled, uses a darker, less intrusive theme"
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