import React, { PropsWithChildren } from "react";

import Spinner from 'react-bootstrap/Spinner';
import Container from "react-bootstrap/Container";

interface Props {
    loaded: boolean
}

export const Loadable = (props: PropsWithChildren<Props>) => {
    if (props.loaded) {
        return props.children;
    } else {
        return (
            <Container fluid style={{textAlign:"center"}}>
                <Spinner animation="border" role="status" size="sm" />
                &nbsp;Loading...
            </Container>
        )
    }
}