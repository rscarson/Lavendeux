import React, { PropsWithChildren, useState } from "react";

import Spinner from 'react-bootstrap/Spinner';
import Container from "react-bootstrap/Container";
import { RootTab } from "../tabs/tab";

interface Props {
    loaded: boolean
}

export const Loadable = (props: PropsWithChildren<Props>) => {
    const [shouldSpinny, setShouldSpinny] = useState<boolean>(false);

    // Only display the spinner if the loading takes longer than 500ms
    setTimeout(() => {
        setShouldSpinny(true);
    }, 500);

    if (props.loaded) {
        return props.children;
    } else if (shouldSpinny) {
        return <RootTab
            tab={""} loaded={true}
            sidebar={<></>}
            content={
                <Container fluid style={{textAlign:"center"}}>
                    <Spinner animation="border" role="status" size="sm" />
                    &nbsp;Loading...
                </Container>
            }
        />
    } else {
        return <RootTab
            tab={""} loaded={true}
            sidebar={<></>}
            content={<></>}
        />
    }
}