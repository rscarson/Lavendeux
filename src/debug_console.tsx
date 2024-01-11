import React, { useEffect, useState } from "react";
import { getCurrent } from "@tauri-apps/api/window";
import { invoke } from "@tauri-apps/api/core";

interface Props {
}

export const DebugConsole = (props: Props) => {
    const [strings, setStrings] = useState<string>('');
    
    useEffect(() => {
        const appWindow = getCurrent();
        appWindow.listen("updated-debug", (event) => {
            let debugOutput = event.payload as Array<string>;
            setStrings(debugOutput.join('<br/>'));
        });
        invoke("read_debug", {}).then(s => setStrings(s ? (s as Array<string>).join('<br/>') : ''));        
    }, [])


    return (
        <pre className="p-3" style={{overflowX: 'scroll', minHeight: "100vh", marginBottom: "0"}}>
            <code dangerouslySetInnerHTML={{__html: strings}}></code>
        </pre>
    );
    }