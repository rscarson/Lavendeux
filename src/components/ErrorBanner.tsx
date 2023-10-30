import React, { PropsWithChildren, useState, useEffect } from "react";

import Alert from 'react-bootstrap/Alert';

interface Props {
    msg: string
}

export const ErrorBanner = (props: Props) => {
    const [show, setShow] = useState(false);

    useEffect(() => {
        if (!props.msg) return;
        setShow(true);
        setTimeout(() => {
            setShow(false);
        }, 2000);
    }, [props.msg]);

    if (!show) return (<></>);
    return (
        <Alert className="fixed-top m-3" variant="danger" onClose={() => setShow(false)} dismissible>
            {props.msg}
        </Alert>
    );
}