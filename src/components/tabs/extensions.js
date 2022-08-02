import React, { useState, useEffect } from 'react';
import { Table, ListGroup } from 'react-bootstrap';

import { IconButton } from '../icon_button';
import { openDialog, listen, run } from '../../include/tauri';

import "./extensions.css"

/**
 * Render the extensions tab
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Extensions(props) {
	const [extensions, setExtensions] = useState([]);

	useEffect(() => {
		listen('extensions', event => {
			setExtensions(event.payload);
		});

        run("reload_all_extensions");
	}, []);

    /**
     * Import an extension through tauri
     */
	function importFile() {
		openDialog()
			.then(filename => run("import_extension", {srcPath: filename}))
			.catch(e => alert(e));
	}

    /**
     * Render an extension as a table row
     * @returns Rendered extension
     */
    function renderExtensionRow(extension) {
        return (
            <tr key={extension.filename}>
                <td>{extension.name} <small className="text-muted">v{extension.version}</small></td>
                <td>{Object.keys(extension.functions).length} functions, {Object.keys(extension.decorators).length} decorators</td>
                <td>{extension.author}</td>
                <td>
			        <IconButton variant="secondary" onClick={() => run("disable_extension", {srcPath: extension.filename})} icon="x-circle" title="Disable" />&nbsp;
                </td>
            </tr>
        );
    }

    /**
     * Render empty extension warning as a table row
     * @returns Rendered data
     */
    function renderExtensionsEmpty() {
        return (
            <tr>
                <td colSpan="4" className="text-center">
                    <strong>No extensions installed</strong>
                </td>
            </tr>
        );
    }

	return (
		<div className="nav-content">
            <ListGroup variant="flush">
                <ListGroup.Item>
			        <IconButton variant="secondary" onClick={() => importFile()} icon="in-up" title="Import Extension" />&nbsp;
			        <IconButton variant="secondary" onClick={() => run("reload_all_extensions")} icon="arrow-clockwise" title="Reload Extensions" />&nbsp;
			        <IconButton variant="secondary" onClick={() => run("open_extensions_dir")} icon="folder2-open" title="Open Extensions Directory" />&nbsp;
                </ListGroup.Item>
                <ListGroup.Item>
                    <Table striped hover>
                        <thead>
                            <tr>
                                <th>Extension</th>
                                <th>About</th>
                                <th>Author</th>
                                <th></th>
                            </tr>
                        </thead>
                        <tbody>
                            {extensions.length 
                                ? extensions.map(extension => renderExtensionRow(extension)) 
                                : renderExtensionsEmpty()
                            }
                        </tbody>
                    </Table>
                </ListGroup.Item>
            </ListGroup>
		</div>
	);
}

export default Extensions;