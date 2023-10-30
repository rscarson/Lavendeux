import React, {useState, useEffect, useReducer} from "react";
import { open } from "@tauri-apps/plugin-shell";
import { invoke } from "@tauri-apps/api/primitives";
import { getCurrent } from "@tauri-apps/api/window";

import { RootTab } from "../Tab";
import { Translated } from "../../components/Translated";
import { Settings } from "../../types";
import { TutorialBlock } from "../../components/tutorial";

import Nav from 'react-bootstrap/Nav';
import Button from 'react-bootstrap/Button';
import { Badge } from "react-bootstrap";
import { Syntax } from "../../components/Syntax/Syntax";

import { HelpData } from "./data";
function format_help_chunk(chunk): React.JSX.Element {
    if (chunk.str) return <p><Translated path={chunk.str} /></p>
    else if (chunk.code) return <Syntax code={chunk.code} />
    else if (chunk.hr) return <hr />;
    else return <></>;
}

const URL_DONATE = "https://www.paypal.com/donate/?business=UXKPLM89RVU5L&no_recurring=0&item_name=Thank+you+for+supporting+open-source+software%21&currency_code=CAD";
const URL_BUGREPORT = "https://github.com/rscarson/Lavendeux/issues/new";

interface Props {}
export const HelpTab: React.FC<Props> = ({}) => {
    const [, forceUpdate] = useReducer(x => x + 1, 0);

    const [loaded, setLoaded] = useState<boolean>(false);
    const [settings, setSettings] = useState<Settings>();
    const [section, setSection] = useState<string>("");
    async function load() {        
        let _settings: Settings = await invoke("read_settings", {});
        if (_settings) {
            setSettings(_settings);
            history && setLoaded(true);
        }
    }

    function changeSection(section: string) {
        setLoaded(false);
        forceUpdate(); setSection(section);
    }    

    useEffect(() => {
        settings && setLoaded(true);
    }, [section]);

    function renderSidebar() {
        return (
            <Nav className="flex-column" variant="pills">
                <Nav.Link active={section==''} className="p-3" onClick={() => changeSection('')}>
                    <i className={"bi bi-1-circle"}>&nbsp;</i>
                    Start Here
                </Nav.Link>

                <hr />
                
                <Nav.Link active={section=='basic_syntax'} className="p-3" onClick={() => changeSection('basic_syntax')}>
                    <i className={"bi bi-2-circle"}>&nbsp;</i>
                    Basic Syntax
                </Nav.Link>
                
                <Nav.Link active={section=='features'} className="p-3" onClick={() => changeSection('features')}>
                    <i className={"bi bi-3-circle"}>&nbsp;</i>
                    Features
                </Nav.Link>
                
                <Nav.Link active={section=='datatypes'} className="p-3" onClick={() => changeSection('datatypes')}>
                    <i className={"bi bi-4-circle"}>&nbsp;</i>
                    Data Types
                </Nav.Link>

                <hr />
                
                <Nav.Link active={section=='operators'} className="p-3" onClick={() => changeSection('operators')}>
                    <i className={"bi bi-5-circle"}>&nbsp;</i>
                    Operators
                </Nav.Link>
                
                <Nav.Link active={section=='functions'} className="p-3" onClick={() => changeSection('functions')}>
                    <i className={"bi bi-6-circle"}>&nbsp;</i>
                    Functions
                </Nav.Link>
                
                <Nav.Link active={section=='decorators'} className="p-3" onClick={() => changeSection('decorators')}>
                    <i className={"bi bi-7-circle"}>&nbsp;</i>
                    Decorators
                </Nav.Link>
                
                <Nav.Link active={section=='extensions'} className="p-3" onClick={() => changeSection('extensions')}>
                    <i className={"bi bi-8-circle"}>&nbsp;</i>
                    Extensions
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
            <Button onClick={() => open(URL_BUGREPORT)}>Click here to report a bug</Button>&nbsp;
            <Button onClick={() => open(URL_DONATE)} variant="primary">Click here support Lavendeux</Button>
        </>);
    }

    function getSection() {
        const title = section.replace('_', ' ');
        if (!section.length) return startSection();
        let data = HelpData[section].map(chunk => format_help_chunk(chunk));

        return (<>
            <h2 style={{textTransform:'capitalize'}}>{title}</h2>
            <hr/>
            {data}
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
};