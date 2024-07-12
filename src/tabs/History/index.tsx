import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";

import { getCurrent } from "@tauri-apps/api/window";
import { save } from "@tauri-apps/plugin-dialog";
import { writeText } from "@tauri-apps/plugin-clipboard-manager";

import { Settings, Snippet } from '../../types';
import { TutorialBlock } from "../../components/tutorial";
import { RootTab } from "../tab";

import Card from 'react-bootstrap/Card';
import Nav from 'react-bootstrap/Nav';
import InputGroup from "react-bootstrap/InputGroup";
import Button from "react-bootstrap/Button";
import { Translated } from "../../components/Translated";
import { Toast } from "react-bootstrap";

const snipPart = (s) => {
    let _s = (s.split("\n")[0]??'').substring(0, 45);
    if (s != _s) _s = `${_s} [...]`;
    return _s;
}

interface Props {}
export const HistoryTab: React.FC<Props> = ({}) => {
    const [loaded, setLoaded] = useState<boolean>(false);
    const [history, setHistory] = useState<Array<Snippet>>([]);
    const [settings, setSettings] = useState<Settings>();
    async function load() {
        let _history: Array<Snippet> = await invoke("read_history", {});
        if (_history) {
            setHistory(_history);
            settings && setLoaded(true);
        }
        
        let _settings: Settings = await invoke("read_settings", {});
        if (_settings) {
            setSettings(_settings);
            history && setLoaded(true);
        }
    }

    function exportHistory() {
        save().then((filename) => invoke("export_history", {destination: filename}));
    }

    function clearHistory() {
        invoke("clear_history", {})
        .then(() => {
            setLoaded(false);
            load();
        });
    }

    function delHistory(id: number) {
        invoke("del_history", {id: id})
        .then(() => {
            setLoaded(false);
            load();
        });
    }
    
    useEffect(() => {
        load();

        const appWindow = getCurrent();
        appWindow.listen("history-updated", (event) => {
          let history = event.payload as Array<Snippet>;
          setHistory(history);
        })
        appWindow.listen("updated-settings", (event) => {
          let settings = event.payload as Settings;
          setSettings(settings);
        })
    }, [])

    function renderSidebar() {
        return (
            <Nav className="flex-column" variant="pills">
                <Nav.Link eventKey="general" className="p-3" onClick={clearHistory}>
                    <i className={"bi bi-x-lg"}>&nbsp;</i>
                    <Translated path="history\btn_clear" />
                </Nav.Link>
                <Nav.Link eventKey="parser" className="p-3" onClick={exportHistory}>
                    <i className={"bi bi-download"}>&nbsp;</i>
                    <Translated path="history\btn_export" />
                </Nav.Link>
            </Nav>
        );
    }

    function itemComponent(icon:string, label: string, text: string, textClass?: string) {
        return (<>
            <InputGroup className="m-1">
                <InputGroup.Text className={`${textClass}`} style={{width:"100px"}}>
                    <i className={`bi bi-${icon}`}>&nbsp;</i>
                    <Translated path={label} />
                </InputGroup.Text>
                <Button variant="secondary" onClick={() => writeText(text).catch((e) => alert(`Could not write to clipboard: ${e}`))}>
                    <i className={"bi bi-clipboard"}></i>
                </Button>
                <InputGroup.Text className="flex-fill" title={text}>
                    <pre className="m-0 p-0">{snipPart(text)}</pre>
                </InputGroup.Text>
            </InputGroup>
        </>);
    }

    function renderItem(snippet: Snippet, id: number) {
        var date = new Date(snippet.timestamp.secs_since_epoch * 1000);

        return (
            <Card className="w-100 mb-3" style={{maxWidth:"750px"}}>
            <Card.Header>
                <small className="text-secondary">{date.toDateString()} {date.toLocaleTimeString()}</small>
                <span
                    className="text-secondary float-end"
                    style={{cursor:"pointer"}}
                    onClick={() => delHistory(id)}
                >
                    <i className="bi bi-x-lg"></i>
                </span>
            </Card.Header>
                <Card.Body>
                    {itemComponent('card-text', 'history\\lbl_input', snippet.input)}
                    {snippet.result.Success && itemComponent('check-lg', 'history\\lbl_output', snippet.result.Success, 'text-success')}
                    {snippet.result.Error && itemComponent('check-lg', 'history\\lbl_error', snippet.result.Error, 'text-danger')}
                </Card.Body>
            </Card>
        );
    }

    function renderContent() {
        return (
            <div className="w-100 pt-3">
                {history.length ? history.map((item,idx) => renderItem(item, idx)) : (
                    <Toast className="w-100" style={{maxWidth: "750px"}}>
                        <Toast.Header closeButton={false}>
                            <Translated path="history\lbl_no_history_to_display" />
                        </Toast.Header>
                        <Toast.Body>
                            <TutorialBlock settings={settings!} />
                        </Toast.Body>
                    </Toast>
                )}
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