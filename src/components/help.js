import React from 'react';
import { Container } from 'react-bootstrap';
import { Samples } from '../include/help_strings';

/**
 * Render the help tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Help(props) {
	return (
		<Container dangerouslySetInnerHTML={
            {__html: 
                Samples.map(s => s.toString()).join('\n')
            }
        }></Container>
	);
}

export default Help;