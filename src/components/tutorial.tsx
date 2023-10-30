import React from "react";

import { KeyboardShortcut, Settings } from "../types";
import { KeyboardShortcutController } from "../util/keyboard_shortcut";
import { Translated } from "./Translated";

import Toast from 'react-bootstrap/Toast';

interface Props {
    settings: Settings,
}

let snippet = "x = 8 + 3e+2\nx = sqrt(16)\nx**3 @hex";
export const TutorialBlock = (props: Props) => {
    const string = props.settings.enhanced_clipboard 
    ? 'general\\components\\tutorial\\lbl_getting_started_ecm'
    : 'general\\components\\tutorial\\lbl_getting_started_noecm';
    return (<>
        <p>
            <Translated path={string} onLoad={(s) => s.replace('{SHORTCUT}', KeyboardShortcutController.toString(props.settings.shortcut))} />
        </p>
        <pre>
            <textarea name="tutblock" rows={3} className="w-100 p-3" defaultValue={snippet} />
        </pre>
    </>);
}