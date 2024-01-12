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
    const [section, setSection] = useState<string>('');

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
                <Nav.Link active={section==''} className="p-3" onClick={() => changeSection('')}>
                    <i className={"bi bi-1-circle"}>&nbsp;</i>
                    <Translated path="help\lbl_getting_started" />
                </Nav.Link>

                <hr />
                
                <Nav.Link active={section=='Chapter 1 - Values'} className="p-3" onClick={() => changeSection('Chapter 1 - Values')}>
                    <i className={"bi bi-2-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Values" />
                </Nav.Link>
                
                <Nav.Link active={section=='Chapter 2 - Operations'} className="p-3" onClick={() => changeSection('Chapter 2 - Operations')}>
                    <i className={"bi bi-3-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Operators" />
                </Nav.Link>
                
                <Nav.Link active={section=='Chapter 3 - Variables, Functions and Loops'} className="p-3" onClick={() => changeSection('Chapter 3 - Variables, Functions and Loops')}>
                    <i className={"bi bi-4-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Features" />
                </Nav.Link>

                <hr />

                <Nav.Link active={section=='Chapter 4 - Extensions'} className="p-3" onClick={() => changeSection('Chapter 4 - Extensions')}>
                    <i className={"bi bi-8-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Extensions" />
                </Nav.Link>
                
                <Nav.Link active={section=='Chapter 5 - Appendix'} className="p-3" onClick={() => changeSection('Chapter 5 - Appendix')}>
                    <i className={"bi bi-5-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Appendix" />
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

    function startSection() {
        return (<>
            <h2>Lavendeux enhances the clipboard</h2>
            <h6>
                Lavendeux inlines calculating, programming and testing utilities into your favourite text editor
            </h6>

            <Badge>
                Select an option from the left to learn more
            </Badge>
            
            <hr/>
            <TutorialBlock settings={settings!} />
            
            <hr/>
            <Button variant="danger" onClick={() => open(URL_BUGREPORT)}>Click here to report a bug</Button>&nbsp;
            <Button onClick={() => open(URL_DONATE)} variant="primary">Click here support Lavendeux</Button>
        </>);
    }

    function getSection() {
        const title = section.replace('_', ' ');
        if (!section.length) return startSection();
        let sectionData = data![section].map(chunk => format_help_chunk(chunk));

        return (<>
            <h2 style={{textTransform:'capitalize'}}>{title}</h2>
            <hr/>
            {sectionData}
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