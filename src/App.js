import React, {useEffect} from 'react';
import {Routes, Route} from "react-router-dom";
import { run, emit } from './include/tauri';

import MainWindow from './components/main_window';
import Logs from './components/logs';
import Error from './components/error';
import './App.css'

import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
	useEffect(() => {		
        run("get_settings")
        .then(e => {
			if (e.dark) {
				document.documentElement.classList.add('dark-theme');
			} else {
				document.documentElement.classList.remove('dark-theme');
			}
		})
        .catch(err => console.log(`Error: ${err}`));

		emit("ready", "payload");
	}, []);

	return (
		<Routes>
			<Route path="/error"
				element={
					<Error />
				}
			/>
			
			<Route path="/logs"
				element={
					<Logs />
				}
			/>

			<Route path="*"
				element={
					<MainWindow />
				}
			/>
		</Routes>
	)
}

export default App