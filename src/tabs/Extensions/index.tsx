import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";

import { getCurrent } from "@tauri-apps/api/window";
import { open } from "@tauri-apps/plugin-dialog";

import { Extension, ExtensionFunction } from '../../types';
import { RootTab } from "../Tab";

import Card from 'react-bootstrap/Card';
import Nav from 'react-bootstrap/Nav';
import Badge from 'react-bootstrap/Badge';

import Button from "react-bootstrap/Button";
import ButtonGroup from "react-bootstrap/ButtonGroup";
import Form from 'react-bootstrap/Form';
import { Translated, translate } from "../../components";

interface Props {}
export const ExtensionsTab: React.FC<Props> = ({}) => {
    const [loaded, setLoaded] = useState<boolean>(false);
    const [extensions, setExtensions] = useState<Map<String, Extension>>(new Map<String, Extension>());

    const [lblToggle, setLblToggle] = useState<string>("extensions\\lbl_toggle");
    const [lblDelete, setLblDelete] = useState<string>("extensions\\lbl_delete");

    async function load() {
        let e: Map<String, Extension> = await invoke("read_extensions", {});
        if (e) {
            setExtensions(e);
            setLoaded(true);
        }
    }

    function disableExtension(filename: string) {
        invoke("disable_extension", {filename});
    }

    function enableExtension(filename: string) {
        invoke("enable_extension", {filename});
    }

    function addExtension() {
        open().then((file) => {
            let filename = file!.path;
            invoke('add_extension', {filename});
        });
    }
    
    useEffect(() => {
        load();

        translate(lblToggle).then(s => setLblToggle(s));
        translate(lblDelete).then(s => setLblDelete(s));

        const appWindow = getCurrent();
        appWindow.listen("updated-extensions", (event) => {
          let e = event.payload as Map<String, Extension>;
          setExtensions(e);
          setLoaded(true);
        })
    }, [])

    function functionSignature(f: ExtensionFunction) {
        if (f.name.startsWith('@')) {
            return `<${f.arguments[0]}> ${f.name}`;
        } else {
            let args = f.arguments.join(', ');
            return `${f.name}(${args}): ${f.returns.toLowerCase()}`;
        }
    }

    function renderExtension(filename: string, extension: Extension) {
        let isEnabled = !!extension.Ok;
        return (
            <Card key={filename} className={`w-100 mb-3 ${extension.Err && 'border-danger'}`} style={{maxWidth:"750px"}} title={isEnabled ? filename : extension.Err}>
                <Card.Header>
                    {isEnabled && <>
                        {extension.Ok!.name}
                        &nbsp;<small className="text-muted">v{extension.Ok!.version}</small>
                    </>}
                    {!isEnabled && <>
                        <span className="text-danger">[disabled] </span>
                        {filename}
                    </>}

                    <span
                        className="text-secondary float-end"
                        style={{cursor:"pointer"}}
                        title={lblDelete}
                        onClick={() => invoke('del_extension', {filename})}
                    >
                        <i className="bi bi-trash"></i>
                    </span>
                    <span
                        className="text-secondary float-end"
                        style={{cursor:"pointer", paddingRight: "5px", paddingTop: "3px"}}
                        title={lblToggle}
                        onClick={() => extension.Err ? enableExtension(filename) : disableExtension(filename)}
                    >      
                        <Form.Check
                        type="switch"
                        name="extension-toggle"
                        className="obvious-toggle"
                        checked={!extension.Err}
                        style={{fontSize: "0.5em"}}
                        />
                    </span>

                    <br />
                    {isEnabled && <small className="text-muted"><Translated path="extensions\lbl_by" /> {extension.Ok!.author}</small>}
                    
                </Card.Header>
                <Card.Body>
                {isEnabled && <>
                        {Object.values(extension.Ok!.functions).map(f => <>
                            <Badge key={f.fname} className="m-1" bg="secondary" title={f.description}>{functionSignature(f)}</Badge>
                        </>)}
                </>}
                {!isEnabled && <small className="text-secondary">{extension.Err}</small>}
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
                
                <Nav.Link className="p-3" onClick={() => {invoke("reload_extensions", {}); setLoaded(false)}}>
                    <i className={"bi bi-arrow-clockwise"}>&nbsp;</i>
                    <Translated path="extensions\btn_reload" />
                </Nav.Link>
            </Nav>
        );
    }

    function renderContent() {
        return (
            <div className="w-100 pt-3">
                {Object.keys(extensions).sort().map(f => renderExtension(f, extensions[f]))}
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