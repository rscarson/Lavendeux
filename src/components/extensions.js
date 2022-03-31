import React, { useState, useEffect } from 'react';
import { Button, Table } from 'react-bootstrap';
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
			<p>
				Extensions are snippets of javascript that can be used to add functionality to Lavendeux.<br/>
				The extensions are run in a secure sandboxed environment.
			</p>
			<p>For an example of how to write an extension, please see the included examples.</p>
			<Button variant="outline-primary" size="sm" onClick={e => importFile()}>Import Extension</Button>&nbsp;
			<Button variant="outline-success" size="sm" onClick={e => run("reload_all_extensions")}>Reload Extensions</Button>&nbsp;
			<Button variant="outline-secondary" size="sm" onClick={e => run("open_extensions_dir")}>Open Extensions Directory</Button>

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
		</div>
	);
}

export default Extensions;