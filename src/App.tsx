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

interface Props {}
export const App: React.FC<Props> = (props) => {
    const [dark, setDark] = useState<boolean>(true);

    const router = createBrowserRouter([
        { path: "/settings?", element: <SettingsTab />},
        { path: "/history", element: <HistoryTab />},
        { path: "/extensions", element: <ExtensionsTab />},
        { path: "/help", element: <HelpTab />},
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

        init_theme();
    }, [])
    
    useEffect(() => {
        document.documentElement.dataset.bsTheme = dark ? 'dark' : '';
    }, [dark])

    return (<>
      <Navbar className="lav-nav">
            <Nav className="me-auto pt-0 pb-0 p-2">
                <Nav.Link active={location.includes('history')} href="/history">History</Nav.Link>
                <Nav.Link active={location.includes('extensions')} href="/extensions">Extensions</Nav.Link>
                <Nav.Link active={location.includes('settings')} href="/settings">Settings</Nav.Link>
                <Nav.Link active={location.includes('help')} href="/help">Help</Nav.Link>
            </Nav>
      </Navbar>
      <Container fluid className="root-container row flex-fill flex-column">
            <RouterProvider router={router} />
      </Container>
    </>);
}