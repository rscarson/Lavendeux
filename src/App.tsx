import React, { useState, useEffect } from "react";
import { createBrowserRouter, RouterProvider, useRouteError } from "react-router-dom";
import { invoke } from "@tauri-apps/api/core";
import { getCurrent } from "@tauri-apps/api/window";
  
import { Nullable, Settings } from "./types";
import { SettingsTab } from './tabs/Settings';
import { HistoryTab } from './tabs/History';
import { ExtensionsTab } from './tabs/Extensions';
import { HelpTab } from "./tabs/Help";
import { DebugConsole } from "./debug_console";

import Container from 'react-bootstrap/Container';
import Nav from 'react-bootstrap/Nav';
import Navbar from 'react-bootstrap/Navbar';

import "./App.css";
import { Translated } from "./components";
import { Button, Card } from "react-bootstrap";

function renderTab(element: React.JSX.Element, activeKey: string, onNav: (string) => void): React.JSX.Element {
    return (<>
        <Navbar className="lav-nav">
              <Nav className="me-auto pt-0 pb-0 p-2">
                  <Nav.Link active={activeKey.includes('history')} onClick={() => onNav('history')}>
                      <Translated path="general\components\pages\lbl_history" />
                  </Nav.Link>
                  <Nav.Link active={activeKey.includes('extensions')} onClick={() => onNav('extensions')}>
                      <Translated path="general\components\pages\lbl_extensions" />
                  </Nav.Link>
                  <Nav.Link active={activeKey.includes('settings')} onClick={() => onNav('settings')}>
                      <Translated path="general\components\pages\lbl_settings" />
                  </Nav.Link>
                  <Nav.Link active={activeKey.includes('help')} onClick={() => onNav('help')}>
                      <Translated path="general\components\pages\lbl_help" />
                  </Nav.Link>
              </Nav>

            <Navbar.Text className="p-2">
                <Button size="sm" variant="outline-light" onClick={() => window.location.reload()} className="m-0">
                    <i className="bi bi-arrow-clockwise">&nbsp;</i>
                </Button>
            </Navbar.Text >
        </Navbar>
        <Container fluid className="root-container row flex-fill flex-column">
                {element}
        </Container>
    </>);
}

interface Props {}
export const App: React.FC<Props> = (props) => {
    const [dark, setDark] = useState<boolean>(true);
    const [activeTab, setActiveTab] = useState<string>("settings");

    // Listen for the ready event
    const [ready, setReady] = useState<boolean>(false);
    useEffect(() => {
        getCurrent().listen("ready", () => {
            setReady(true);
        });

        invoke("is_ready", {}).then((ready) => {
            if (ready as boolean == true) {
                setReady(true);
            }
        });
    }, []);

    const [settingsTab, setSettingsTab] = useState<React.JSX.Element>(<></>);
    const [historyTab, setHistoryTab] = useState<React.JSX.Element>(<></>);
    const [extensionsTab, setExtensionsTab] = useState<React.JSX.Element>(<></>);
    const [helpTab, setHelpTab] = useState<React.JSX.Element>(<></>);
    useEffect(() => {
        if (ready) {
            setSettingsTab(<SettingsTab />);
            setHistoryTab(<HistoryTab />);
            setExtensionsTab(<ExtensionsTab />);
            setHelpTab(<HelpTab />);
        }
    }, [ready]);

    // @todo 'PRIVATE - DO NOT USE' on this... oops
    let location = window.location.pathname;
    if (location == "/debug") {
        return <DebugConsole />;
    }

    async function init_theme() {
        const cachedValue = localStorage.getItem("lav-theme");
        if (cachedValue !== null && cachedValue != "null") {
            setDark(cachedValue == "true");
        }

        let settings: Nullable<Settings> = await invoke("read_settings", {});
        if (settings) {
            localStorage.setItem("lav-theme", settings.dark_theme ? "true" : "false");
            setDark(settings.dark_theme);
        }
    }
    
    useEffect(() => {
        if (!ready) return;
        const appWindow = getCurrent();
        appWindow.listen("updated-settings", (event) => {
            let settings = event.payload as Settings;
            setDark(settings.dark_theme);
        })

        appWindow.listen("tab-request", (event) => {
            let target = event.payload as string;
            window.location.href = target;
        })

        init_theme();
    }, [ready])
    
    useEffect(() => {
        document.documentElement.dataset.bsTheme = dark ? 'dark' : '';
    }, [dark]);

    function getTabContent(location: string) {
        switch (location) {
            case "settings":
                return renderTab(settingsTab, "settings", setActiveTab);
            case "history":
                return renderTab(historyTab, "history", setActiveTab);
            case "extensions":
                return renderTab(extensionsTab, "extensions", setActiveTab);
            case "help":
                return renderTab(helpTab, "help", setActiveTab);
            default:
                return renderTab(settingsTab, "settings", setActiveTab);
        }
    }

    return ready ? <>
        <ErrorBoundary onNav={setActiveTab}>
            {getTabContent(activeTab)}
        </ErrorBoundary>
    </> : renderTab(<></>, "", setActiveTab);
}

interface ErrorBoundaryProps {
    onNav: (string) => void;
    children: React.ReactNode;
}

class ErrorBoundary extends React.Component<ErrorBoundaryProps> {
    state = { hasError: false, error: null };

    constructor(props: any) {
        super(props);
    }

    static getDerivedStateFromError(error: any) {
        return { hasError: true, error: error};
    }

    componentDidCatch(error: any, errorInfo: any) {
        console.error(error);
    }

    render() {
        if (this.state.hasError) {
            let error = this.state.error! as Error;
            return renderTab(
                <Container fluid className="root-container">
                    <Card className="m-3" border="danger">
                        <Card.Header>An Error has occurred</Card.Header>
                        <Card.Body>
                            <Card.Title>{error.message}</Card.Title>
                            <Card.Text>
                                You can try selecting a different tab, or reloading the application.
                            </Card.Text>
                        </Card.Body>
                    </Card>
            
                    <Card style={{borderRadius: 0}}>
                        <Card.Header>Debug Information</Card.Header>
                        <Card.Body>
                            <pre>{error.stack}</pre>
                        </Card.Body>
                    </Card>
                </Container>
            , "error", (s) => {
                this.setState({ hasError: false });
                this.props.onNav(s);
            });
        }

        return this.props.children;
    }
}