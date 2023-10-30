import React from "react";

import { Loadable } from '../components';

import Row from "react-bootstrap/Row";
import Col from "react-bootstrap/Col";
import Tab from "react-bootstrap/Tab";

interface Props {
    tab?: string,
    sidebar: React.JSX.Element,
    content: React.JSX.Element,
    loaded: boolean,
}

export const RootTab: React.FC<Props> = (props) => {
    return(<>
        <Row className="flex-grow-1 root-container">
            <Tab.Container activeKey={props.tab??''}>
                <Col md={3} className="bg-body-secondary nav-sidebar">
                    {props.sidebar}
                </Col>
                <Col className="d-flex justify-content-center pt-3">
                    <Loadable loaded={props.loaded}>
                        {props.content}
                    </Loadable>
                </Col>
            </Tab.Container>
        </Row>
    </>);
};