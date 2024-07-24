import React, { useState, useEffect } from "react";

import Modal from 'react-bootstrap/Modal';
import Button from 'react-bootstrap/Button';

import { Translated, translate } from "../../components";

interface Props {
    onConfirm: (restart: boolean) => void;
}

/// A modal dialog that asks the user to confirm an extension deletion.
/// It will notify the user that the extension will be removed from the system
/// And that changes will not be made until the parser is restarted.
/// It will have "Cancel", Delete, and "Delete and Restart" buttons.
export const ExtensionDeletionConfirmation: React.FC<Props> = (props) => {
    const [show, setShow] = useState<boolean>(false);
    const [lblDelete, setLblDelete] = useState<string>("extensions\\lbl_delete");
    
    useEffect(() => {
        translate(lblDelete).then(s => setLblDelete(s));
    }, [])

    function del(restart: boolean = false) {
        setShow(false);
        props.onConfirm(restart);
    }

    return (<>
    
        <span
            className="text-secondary float-end"
            style={{cursor:"pointer"}}
            title={lblDelete}
            onClick={() => setShow(true)}
        >
            <i className="bi bi-trash"></i>
        </span>
        <Modal show={show} onHide={() => setShow(false)}>
            <Modal.Header closeButton>
                <Modal.Title>
                    <Translated path="extensions\lbl_delete" />
                </Modal.Title>
            </Modal.Header>
            <Modal.Body>
                <p>
                    <Translated path="extensions\lbl_confirm_delete" />
                </p>
                <p>
                    <Translated path="extensions\lbl_confirm_delete_restart" />
                </p>
            </Modal.Body>
            <Modal.Footer>
                <Button variant="secondary" onClick={() => setShow(false)}>
                    <Translated path="extensions\btn_cancel" />
                </Button>
                <Button variant="danger" onClick={() => del(false)}>
                    <Translated path="extensions\btn_delete" />
                </Button>
                <Button variant="danger" onClick={() => del(true)}>
                    <Translated path="extensions\btn_delete_restart" />
                </Button>
            </Modal.Footer>
        </Modal>
    </>);
}