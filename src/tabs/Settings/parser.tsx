import React, { useState } from "react";

import { TextboxSetting, SwitchSetting } from "./setting_container";
import { Translated } from "../../components";

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
                icon="clipboard-plus" title={<Translated path="settings\parsing\lbl_ecm" />}
                desc={<Translated path="settings\parsing\desc_ecm" />}
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
                icon="file-earmark-code" title={<Translated path="settings\parsing\lbl_script" />}
                desc={<Translated path="settings\parsing\desc_script" />}
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