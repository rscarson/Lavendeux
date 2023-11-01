import React, {useState, useEffect, useReducer} from "react";
import { open } from "@tauri-apps/plugin-shell";
import { invoke } from "@tauri-apps/api/primitives";
import { getCurrent } from "@tauri-apps/api/window";

import { RootTab } from "../Tab";
import { MarkdownToken, MarkdownTree, Settings } from "../../types";
import { TutorialBlock } from "../../components/tutorial";

import Nav from 'react-bootstrap/Nav';
import Button from 'react-bootstrap/Button';
import { Badge } from "react-bootstrap";
import { Syntax } from "../../components/Syntax/Syntax";
import { Translated } from "../../components";

function format_help_chunk(chunk): React.JSX.Element {
    if (chunk.PlainText) return <p>{chunk.PlainText}</p>
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
                
                <Nav.Link active={section=='Basic Syntax'} className="p-3" onClick={() => changeSection('Basic Syntax')}>
                    <i className={"bi bi-2-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Basic Syntax" />
                </Nav.Link>
                
                <Nav.Link active={section=='Features'} className="p-3" onClick={() => changeSection('Features')}>
                    <i className={"bi bi-3-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Features" />
                </Nav.Link>
                
                <Nav.Link active={section=='Datatypes'} className="p-3" onClick={() => changeSection('Datatypes')}>
                    <i className={"bi bi-4-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Datatypes" />
                </Nav.Link>

                <hr />
                
                <Nav.Link active={section=='Operators'} className="p-3" onClick={() => changeSection('Operators')}>
                    <i className={"bi bi-5-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Operators" />
                </Nav.Link>
                
                <Nav.Link active={section=='Functions'} className="p-3" onClick={() => changeSection('Functions')}>
                    <i className={"bi bi-6-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Functions" />
                </Nav.Link>
                
                <Nav.Link active={section=='Decorators'} className="p-3" onClick={() => changeSection('Decorators')}>
                    <i className={"bi bi-7-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Decorators" />
                </Nav.Link>
                
                <Nav.Link active={section=='Extensions'} className="p-3" onClick={() => changeSection('Extensions')}>
                    <i className={"bi bi-8-circle"}>&nbsp;</i>
                    <Translated path="help\sections\Extensions" />
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