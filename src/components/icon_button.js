import React from 'react';

import { Button } from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

/**
 * Render a button with an icon
 * @param {Object} props Component properties
 * @returns JSX
 */
 export function IconButton(props) {
	return (
		<Button variant={`outline-${props.variant}`} size="sm" onClick={props.onClick}>
            <i class={`bi bi-${props.icon}`}>{props.title && " "}</i>
            {props.title}
        </Button>
	);
}

export default IconButton;