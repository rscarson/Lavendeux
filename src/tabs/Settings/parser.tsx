import React, { useState } from "react";
import { TextboxSetting, SwitchSetting } from "./setting_container";

import Form from 'react-bootstrap/Form';

interface Props {
    onChangeEnhancedClipboard: Function,
    enhancedClipboard: boolean,

    startScript: string,
    onChangeStartScript: Function,
}

export const ParserSettings = (props:Props) => {
    function renderEnhancedClipboard() {
        return (
            <SwitchSetting
                icon="clipboard-plus" title="Enable enhanced clipboard mode"
                desc="When enabled, replaces highlighted text directly, bypassing the clipboard. Otherwise, replaces the clipboard's contents"
                id="enhanced-clipboard" checked={props.enhancedClipboard} 
                onChange={(e) => {
                    let elem = e.target as HTMLInputElement;
                    props.onChangeEnhancedClipboard(elem.checked);
                }}
            />
        );
    }
    
    function renderStartScript() {
        return (
            <TextboxSetting
                icon="file-earmark-code" title="Start script"
                desc="A Lavendeux script to run every time the parser starts up"
                id="enhanced-clipboard" value={props.startScript} 
                onChange={(e) => {
                    let elem = e.target as HTMLInputElement;
                    props.onChangeStartScript(elem.value);
                }}
            />
        );
    }

    return (<>
        {renderEnhancedClipboard()}
        {renderStartScript()}
    </>);
}