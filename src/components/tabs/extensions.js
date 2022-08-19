import React, { useState, useEffect } from 'react';
import { Table, ListGroup } from 'react-bootstrap';

import { IconButton } from '../icon_button';
import { openDialog, listen, run } from '../../include/tauri';

import "./css/extensions.css"

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

        reload_extensions();
	}, []);

    /**
     * Reload the list of available extensions
     */
    function reload_extensions() {
        run("reload_all_extensions").then(e => {
			setExtensions(e);
		});
    }

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
                <td>
                    {Object.keys(extension.functions).length} {props.lang.extensionview_functions}
                    , {Object.keys(extension.decorators).length} {props.lang.extensionview_decorators}
                </td>
                <td>{extension.author}</td>
                <td>
			        <IconButton variant="secondary" onClick={() => run("disable_extension", {srcPath: extension.filename})} 
                        icon="x-circle" title={props.lang.extensionview_btn_disable} />&nbsp;
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
                    <strong>{props.lang.extensionview_no_extensions}</strong>
                </td>
            </tr>
        );
    }

	return (
		<div className="nav-content">
            <ListGroup variant="flush">
                <ListGroup.Item>
			        <IconButton variant="secondary" onClick={importFile} icon="box-arrow-in-up" title={props.lang.extensionview_btn_import} />&nbsp;
			        <IconButton variant="secondary" onClick={reload_extensions} icon="arrow-clockwise" title={props.lang.extensionview_btn_reload} />&nbsp;
			        <IconButton variant="secondary" onClick={() => run("open_extensions_dir")} icon="folder2-open" title={props.lang.extensionview_btn_open} />&nbsp;
                </ListGroup.Item>
                <ListGroup.Item>
                    <Table striped hover>
                        <thead>
                            <tr>
                                <th>{props.lang.extensionview_extension}</th>
                                <th>{props.lang.extensionview_about}</th>
                                <th>{props.lang.extensionview_author}</th>
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