import React from 'react';
import { Container } from 'react-bootstrap';
import { helpText } from '../include/help_strings';

/**
 * Render the help tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Help(props) {
	return (
		<Container dangerouslySetInnerHTML={
            {__html: helpText()}
        }></Container>
	);
}

export default Help;