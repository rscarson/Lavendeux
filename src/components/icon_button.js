import React from 'react';

import { Button } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

/**
 * Render a button with an icon
 * Expects the variant, onClick, icon, and title props
 * @param {Object} props Component properties
 * @returns JSX
 */
 export function IconButton(props) {
    let DEFAULT_VARIANT = "secondary";

	return (
		<Button variant={`outline-${props.variant || DEFAULT_VARIANT}`} size="sm" onClick={() => props.onClick && props.onClick()}>
            <i className={`bi bi-${props.icon}`}>{props.title && " "}</i>
            {props.title}
        </Button>
	);
}

export default IconButton;