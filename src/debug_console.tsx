import React, { useEffect, useState } from "react";
import { getCurrent } from "@tauri-apps/api/window";
import { invoke } from "@tauri-apps/api/core";
import { Badge, ListGroup } from "react-bootstrap";

interface Props {
}

export const DebugConsole = (props: Props) => {
    const [strings, setStrings] = useState<Array<string>>(new Array<string>());
    
    useEffect(() => {
        const appWindow = getCurrent();
        appWindow.listen("updated-debug", (event) => {
            let debugOutput = event.payload as Array<string>;
            setStrings(debugOutput);
        });
        invoke("read_debug", {}).then(s => setStrings(s ? (s as Array<string>) : []));        
    }, [])


    return (<ListGroup className="p-3" style={{overflowX: 'scroll', minHeight: "100vh", marginBottom: "0"}}>
        {strings.map((s, i) => <>
            <ListGroup.Item key={i}>
                <pre>
                    <code>
                        <Badge bg="secondary">{i}</Badge> &nbsp;
                        {s}
                    </code>
                </pre>
            </ListGroup.Item>
        </> )}
    </ListGroup>);
}