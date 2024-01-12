import React, { useState, useEffect } from "react";
import { createBrowserRouter, RouterProvider } from "react-router-dom";
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

interface Props {}
export const App: React.FC<Props> = (props) => {
    const [dark, setDark] = useState<boolean>(true);

    const _settings = <SettingsTab />;
    const _history = <HistoryTab />;
    const _extensions = <ExtensionsTab />;
    const _help = <HelpTab />;

    const router = createBrowserRouter([
        { path: "/settings?", element: renderTab(_settings, "settings")},
        { path: "/history", element: renderTab(_history, "history")},
        { path: "/extensions", element: renderTab(_extensions, "extensions")},
        { path: "/help", element: renderTab(_help, "help")},
        { path: "/debug", element: <DebugConsole />},
    ]);
    // @todo 'PRIVATE - DO NOT USE' on this... oops
    let location = router.state.location.pathname;

    function renderTab(element: React.JSX.Element, activeKey: string): React.JSX.Element {
        return (<>
            <Navbar className="lav-nav">
                  <Nav className="me-auto pt-0 pb-0 p-2">
                      <Nav.Link active={activeKey.includes('history')} href="/history">
                          <Translated path="general\components\pages\lbl_history" />
                      </Nav.Link>
                      <Nav.Link active={activeKey.includes('extensions')} href="/extensions">
                          <Translated path="general\components\pages\lbl_extensions" />
                      </Nav.Link>
                      <Nav.Link active={activeKey.includes('settings')} href="/settings">
                          <Translated path="general\components\pages\lbl_settings" />
                      </Nav.Link>
                      <Nav.Link active={activeKey.includes('help')} href="/help">
                          <Translated path="general\components\pages\lbl_help" />
                      </Nav.Link>
                  </Nav>
            </Navbar>
            <Container fluid className="root-container row flex-fill flex-column">
                    {element}
            </Container>
        </>);
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
    }, [])
    
    useEffect(() => {
        document.documentElement.dataset.bsTheme = dark ? 'dark' : '';
    }, [dark])

    return <RouterProvider router={router} />;
}