import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";

import { getCurrent } from "@tauri-apps/api/window";
import { open } from "@tauri-apps/plugin-dialog";

import { Extension, FunctionDocs } from '../../types';
import { RootTab } from "../tab";

import Card from 'react-bootstrap/Card';
import Nav from 'react-bootstrap/Nav';
import Badge from 'react-bootstrap/Badge';

import Alert from 'react-bootstrap/Alert';
import Button from "react-bootstrap/Button";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Form from 'react-bootstrap/Form';

import { Translated, translate } from "../../components";
import { ExtensionDeletionConfirmation } from "./confirmation";

interface Props {}
export const ExtensionsTab: React.FC<Props> = ({}) => {
    const [loaded, setLoaded] = useState<boolean>(false);
    const [extensions, setExtensions] = useState<Array<Extension>>(new Array<Extension>());

    const [lblToggle, setLblToggle] = useState<string>("extensions\\lbl_toggle");

    async function load() {
        let e: Array<Extension> = await invoke("read_extensions", {});
        if (e) {
            setExtensions(e);
            setLoaded(true);
        }
    }

    function addExtension() {
        open().then((file) => {
            let filename = file!.path;
            invoke('add_extension', {filename})
            .then(() => {
                setLoaded(false);
                load();
            });
        });
    }

    function restartParser() {
        invoke('restart_parser', {})
        .then(() => {
            setLoaded(false);
            load();
        })
    }

    function deleteExtension(id: number, restart: boolean) {
        invoke('del_extension', {id})
        .then(() => {
            if (restart) restartParser()        
        })
    }
    
    useEffect(() => {
        load();

        translate(lblToggle).then(s => setLblToggle(s));

        const appWindow = getCurrent();
        appWindow.listen("updated-extensions", (event) => {
          let e = event.payload as Array<Extension>;
          setExtensions(e);
          setLoaded(true);
        })
    }, [])

    function renderExtension(extension: Extension) {
        let id = extension.id;
        let path = extension.module.filename;
        let filename = path.split('\\').pop() ?? path
        let version = extension.metadata.version ?? "0.0.0";
        let author = extension.metadata.author ?? "@anon";

        let name = extension.metadata.name ?? filename;
        
        return (
            <Card 
                key={id} 
                className={`w-100 mb-3 ${extension.problem !== null && 'border-danger'}`} 
                style={{maxWidth:"585px"}} 
                title={filename}
            >
                <Card.Header>
                    {extension.problem !== null && <span className="text-danger">[error] </span>}

                    <>
                        <a title={`eid-${id}`}>{name}</a>
                        &nbsp;<small className="text-muted">v{version}</small>
                    </>

                    {extension.metadata.license && <Badge bg="" text="secondary">{extension.metadata.license}</Badge>}

                    <ExtensionDeletionConfirmation onConfirm={(restart) => deleteExtension(id, restart)} />

                    <br />
                    {<small className="text-muted"><Translated path="extensions\lbl_by" /> {author}</small>}
                    
                </Card.Header>
                <Card.Body>
                    {extension.metadata.description && <h6 className="text-muted">{extension.metadata.description}</h6>}
                    {extension.problem !== null && <Alert variant="danger">{extension.problem}</Alert>}
        
                    {Object.values(extension.functions).map(f => <>
                        <Badge key={f.signature} className="m-1" bg="secondary" title={f.description ?? ''}>{f.signature}</Badge>
                    </>)}
                </Card.Body>
            </Card>
        );
    }

    function renderSidebar() {
        return (
            <Nav className="flex-column" variant="pills">
                <Nav.Link className="p-3" onClick={() => addExtension()}>
                    <i className={"bi bi-plus-lg"}>&nbsp;</i>
                    <Translated path="extensions\btn_import" />
                </Nav.Link>

                <Nav.Link className="p-3" onClick={() => invoke("open_ext_dir", {})}>
                    <i className={"bi bi-folder2-open"}>&nbsp;</i>
                    <Translated path="extensions\btn_browse" />
                </Nav.Link>
                
                <Nav.Link className="p-3" onClick={() => restartParser()}>
                    <i className={"bi bi-arrow-clockwise"}>&nbsp;</i>
                    <Translated path="extensions\btn_restart" />
                </Nav.Link>
            </Nav>
        );
    }

    function renderContent() {
        return (
            <div className="w-100 pt-3">
                {extensions.map((extension, idx) => renderExtension(extension))}
            </div>
        );
    }

    return(
        <RootTab
            loaded={loaded}
            sidebar={renderSidebar()}
            content={renderContent()}
        />
    );
};