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
	const DEFAULT_VARIANT = 'secondary';
	const {
		variant = DEFAULT_VARIANT,
		onClick = () => {},
		icon = '',
		title = '',
		disabled = false,
	} = props;

	return (
		<Button variant={`outline-${variant}`} size="sm" onClick={onClick} disabled={disabled}>
			<i className={`bi bi-${icon}`}>{title.length ? ' ' : ''}</i>
			{title}
		</Button>
	);
}

export default IconButton;
