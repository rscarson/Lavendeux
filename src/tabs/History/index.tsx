import React, { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/tauri";

import { Settings, KeyboardShortcut, Nullable } from '../../types';

import Card from 'react-bootstrap/Card';
import Nav from 'react-bootstrap/Nav';
import Form from 'react-bootstrap/Form';
import Row from "react-bootstrap/Row";
import Col from "react-bootstrap/Col";
import Tab from "react-bootstrap/Tab";
import Tabs from "react-bootstrap/Tabs";
import { Loadable } from "../../components";

interface Props {}

export const HistoryTab: React.FC<Props> = ({}) => {
    const [loaded, setLoaded] = useState<boolean>(false);
    async function load() {
        let settings: Nullable<Settings> = await invoke("read_settings", {});
        if (settings) {
            setLoaded(true);
        }
    }
    
    useEffect(() => {
        load();
    }, [])

    return(<>
        <Row className="bg-body-secondary pt-2">
            <Col className="m-2"></Col>
        </Row>
        <Row>
            <Col md={2} className="bg-body-secondary nav-sidebar">
                
            </Col>
            <Col className="d-flex justify-content-center mb-4">
                <Loadable loaded={loaded}>
                    
                </Loadable>
            </Col>
        </Row>
    </>);
};