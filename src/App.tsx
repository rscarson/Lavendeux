import React, { useState, useEffect } from "react";
import { createBrowserRouter, RouterProvider } from "react-router-dom";

import { invoke } from "@tauri-apps/api/primitives";
import { getCurrent } from "@tauri-apps/api/window";
  
import { Nullable, Settings } from "./types";
import { SettingsTab } from './tabs/Settings';
import { HistoryTab } from './tabs/History';
import { ExtensionsTab } from './tabs/Extensions';
import { HelpTab } from "./tabs/Help";

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
        { path: "/settings?", element: _settings},
        { path: "/history", element: _history},
        { path: "/extensions", element: _extensions},
        { path: "/help", element: _help},
    ]);
    // @todo 'PRIVATE - DO NOT USE' on this... oops
    let location = router.state.location.pathname;

    async function init_theme() {
        let settings: Nullable<Settings> = await invoke("read_settings", {});
        if (settings) setDark(settings.dark_theme);
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

    return (<>
      <Navbar className="lav-nav">
            <Nav className="me-auto pt-0 pb-0 p-2">
                <Nav.Link active={location.includes('history')} href="/history">
                    <Translated path="general\components\pages\lbl_history" />
                </Nav.Link>
                <Nav.Link active={location.includes('extensions')} href="/extensions">
                    <Translated path="general\components\pages\lbl_extensions" />
                </Nav.Link>
                <Nav.Link active={location.includes('settings')} href="/settings">
                    <Translated path="general\components\pages\lbl_settings" />
                </Nav.Link>
                <Nav.Link active={location.includes('help')} href="/help">
                    <Translated path="general\components\pages\lbl_help" />
                </Nav.Link>
            </Nav>
      </Navbar>
      <Container fluid className="root-container row flex-fill flex-column">
            <RouterProvider router={router} />
      </Container>
    </>);
}