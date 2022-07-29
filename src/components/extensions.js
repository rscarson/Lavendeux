import React, { useState, useEffect } from 'react';
import { Button, Table, ListGroup } from 'react-bootstrap';
import { openDialog, importExtension, disableExtension, listen, run } from '../include/tauri';

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
			.then(filename => importExtension(filename))
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
                    <Button variant="outline-secondary" size="sm" onClick={e => disableExtension(extension.filename)}>
                        Disable Extension
                    </Button>
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
                    <Button variant="outline-secondary" size="sm" onClick={e => importFile()}>
                        <i class="bi bi-box-arrow-in-up">&nbsp;</i>
                        Import Extension
                    </Button>&nbsp;
                    <Button variant="outline-secondary" size="sm" onClick={e => run("reload_all_extensions")}>
                        <i class="bi bi-arrow-clockwise">&nbsp;</i>
                        Reload Extensions
                    </Button>&nbsp;
                    <Button variant="outline-secondary" size="sm" onClick={e => run("open_extensions_dir")}>
                        <i class="bi bi-folder2-open">&nbsp;</i>
                        Open Extensions Directory
                    </Button>
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