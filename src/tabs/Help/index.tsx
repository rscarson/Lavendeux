import React, {useState, useEffect, useReducer} from "react";
import { open } from "@tauri-apps/plugin-shell";
import { invoke } from "@tauri-apps/api/core";
import { getCurrent } from "@tauri-apps/api/window";

import { RootTab } from "../tab";
import { MarkdownToken, MarkdownTree, Settings } from "../../types";
import { TutorialBlock } from "../../components/tutorial";

import Nav from 'react-bootstrap/Nav';
import Button from 'react-bootstrap/Button';
import { Badge } from "react-bootstrap";
import { Syntax } from "../../components/Syntax/Syntax";
import { Translated } from "../../components";

/**
 * Will render:`content` as <pre><code>content</code></pre>
 * *content* as italics
 * **content** as bold
 * ***content*** as bold italics
 */
function format_md_plaintext(text: string): string {
    let _text = text;
    _text = _text.replace(/\*\*\*(.*?)\*\*\*/g, '<b><i>$1</i></b>');
    _text = _text.replace(/\*\*(.*?)\*\*/g, '<b>$1</b>');
    _text = _text.replace(/\*(.*?)\*/g, '<i>$1</i>');
    _text = _text.replace(/`(.*?)`/g, '<code>$1</code>');
    return _text;
}

function format_help_chunk(chunk): React.JSX.Element {
    if (chunk.PlainText) return <p dangerouslySetInnerHTML={{__html: format_md_plaintext(chunk.PlainText)}}></p>
    else if (chunk.H3) return <h3 style={{borderBottom: '1px solid var(--bs-secondary-border-subtle)'}}>{chunk.H3}</h3>
    else if (chunk.H4) return <h4>{chunk.H4}</h4>
    else if (chunk.UL) return <ul>{chunk.UL.map(item => <li dangerouslySetInnerHTML={{__html: format_md_plaintext(item)}}></li>)}</ul>
    else if (chunk.CodeBlock) return <Syntax code={chunk.CodeBlock[1]} />
    else if (chunk.HR) return <hr />;
    else return <></>;
}

const URL_DONATE = "https://www.paypal.com/donate/?business=UXKPLM89RVU5L&no_recurring=0&item_name=Thank+you+for+supporting+open-source+software%21&currency_code=CAD";
const URL_BUGREPORT = "https://github.com/rscarson/Lavendeux/issues/new";

interface Props {};
export const HelpTab: React.FC<Props> = ({}) => {
    const [, forceUpdate] = useReducer(x => x + 1, 0);

    const [loaded, setLoaded] = useState<boolean>(false);
    const [settings, setSettings] = useState<Settings>();
    const [data, setData] = useState<Map<string, Array<MarkdownToken>>>();
    const [section, setSection] = useState<string>('Getting Started');

    async function load() {        
        let _settings: Settings = await invoke("read_settings", {});
        if (_settings) {
            setSettings(_settings);
            
            let _data: MarkdownTree = await invoke("help_text", {});
            if (_data) {
                console.log(_data);
                setLoaded(true);
                setData(_data.subheadings);
            }
        }
    }

    function changeSection(_section: string) {
        if (section === _section) return;
        setLoaded(false);
        forceUpdate(); setSection(_section);
    }    

    useEffect(() => {
        settings && setLoaded(true);
    }, [section]);

    function renderSidebar() {
        return (
            <Nav className="flex-column" variant="pills">
                <Nav.Link active={section==''} className="p-3" onClick={() => changeSection('Getting Started')}>
                    <i className={"bi bi-1-circle"}>&nbsp;</i>
                    <Translated path="help\lbl_getting_started" />
                </Nav.Link>

                <Nav.Link active={section=='Extensions'} className="p-3" onClick={() => changeSection('Extensions')}>
                    <i className={"bi bi-plugin"}>&nbsp;</i>
                    <Translated path="help\sections\Extensions" />
                </Nav.Link>
                
                <Nav.Link active={section=='Functions'} className="p-3" onClick={() => changeSection('Functions')}>
                    <i className={"bi bi-book"}>&nbsp;</i>
                    <Translated path="help\sections\Functions" />
                </Nav.Link>
            </Nav>
        );
    }
    
    useEffect(() => {
        load();

        const appWindow = getCurrent();
        appWindow.listen("updated-settings", (event) => {
          let settings = event.payload as Settings;
          setSettings(settings);
        })
    }, [])

    function sectionFooter() {
        return (<>
            <hr/>
            <TutorialBlock settings={settings!} />
            
            <hr/>
            <Button variant="danger" onClick={() => open(URL_BUGREPORT)}>Click here to report a bug</Button>&nbsp;
            <Button onClick={() => open(URL_DONATE)} variant="primary">Click here support Lavendeux</Button>
        </>);
    }

    function getSection() {
        if (data === undefined) return <></>;

        const title = section.replace('_', ' ');
        let sectionData = data![section].map(chunk => format_help_chunk(chunk));

        return (<>
            <h2 style={{textTransform:'capitalize'}}>{title}</h2>
            <hr/>
            {sectionData}

            {sectionFooter()}
        </>);
    }

    function renderContent() {
        return (
            <div className="w-100 pt-3">
                {getSection()}
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
}