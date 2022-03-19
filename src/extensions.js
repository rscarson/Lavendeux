import React from 'react';
import { Button, Table } from 'react-bootstrap';
import { open } from "@tauri-apps/api/dialog"

function Extensions(props) {
	function importFile() {
		open()
			.then(filename => props.onImport(filename))
			.catch(e => alert(e));
	}

	return (
		<div className="nav-content">
			<p>
				Extensions are snippets of javascript that can be used to add functionality to Lavendeux.<br/>
				The extensions are run in a secure sandboxed environment.
			</p>
			<p>For an example of how to write an extension, please see the included examples.</p>
			<Button variant="outline-primary" size="sm" onClick={e => importFile()}>Import Extension</Button>&nbsp;
			<Button variant="outline-success" size="sm" onClick={e => props.onReload()}>Reload Extensions</Button>&nbsp;
			<Button variant="outline-secondary" size="sm" onClick={e => props.onOpen()}>Open Extensions Directory</Button>

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
					{props.extensions.length ? props.extensions.map(extension => (
					<tr key={extension.filename}>
						<td>{extension.name} <small className="text-muted">v{extension.version}</small></td>
						<td>{Object.keys(extension.functions).length} functions, {Object.keys(extension.decorators).length} decorators</td>
						<td>{extension.author}</td>
						<td>
							<Button variant="outline-secondary" size="sm" onClick={e => props.onDisable(extension.filename)}>
								Disable Extension
							</Button>
						</td>
					</tr>
					)) : (
						<tr>
							<td colSpan="4" className="text-center">
								<strong>No extensions installed</strong>
							</td>
						</tr>
					)}
				</tbody>
			</Table>
		</div>
	);
}

export default Extensions;