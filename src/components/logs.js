import React, { useState, useEffect } from 'react';
import { Table, ListGroup, Button } from 'react-bootstrap';
import { listen, run } from '../include/tauri';

/**
 * Render the log viewer
 * @param {Object} props Component properties
 * @returns Component contents
 */
function Logs(props) {
	const [logs, setLogs] = useState([]);

	useEffect(() => {
		listen('logs', event => {
			setLogs(event.payload);
		});


        run("get_logs")
        .then(e => setLogs(e))
        .catch(err => console.log(`Error: ${err}`));
	}, []);

    function renderRow(row) {
        return (     
        <tr key={row.timestamp}>
            <td>{row.timestamp}</td>
            <td>{row.level}</td>
            <td>{row.text}</td>
        </tr>
        )
    }

    function renderEmptyLog(row) {
        return (     
        <tr>
            <td colSpan={3} className="text-center">
                Nothing to display
            </td>
        </tr>
        )
    }

	return (<>
        <ListGroup variant="flush">
            <ListGroup.Item>
                <Button onClick={e => run("clear_logs")} variant="outline-secondary" size="sm">
                    <i class="bi bi-clock-history">&nbsp;</i>
                    Clear Log
                </Button>&nbsp;
                    <Button variant="outline-secondary" size="sm" onClick={e => run("open_logs_dir")}>
                        <i class="bi bi-folder2-open">&nbsp;</i>
                        Open Log Directory
                    </Button>
            </ListGroup.Item>
            
            <Table className="w-100" striped hover>
                <thead>
                    <tr>
                        <th className="col-sm-2">Timestamp</th>
                        <th>Level</th>
                        <th>Event</th>
                    </tr>
                </thead>
                <tbody>
                    { logs.length 
                        ? logs.map(entry => renderRow(entry)) 
                        : renderEmptyLog()
                    }
                </tbody>
            </Table>
        </ListGroup>
    </>);
}

export default Logs;