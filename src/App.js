import React from 'react';
import {Routes, Route} from "react-router-dom";

import MainWindow from './windows/main';
import LogsWindow from './windows/logs';
import ErrorWindow from './windows/error';

import 'bootstrap-icons/font/bootstrap-icons.css';
import 'bootstrap/dist/css/bootstrap.min.css';
import './App.css'
import './dark_theme.css'

/**
 * Render a route in the app
 * @param {string} path 
 * @param {Component} Child 
 * @returns 
 */
function renderRoute(path, Child) {
	return (
		<Route path={path}
			element={
				<Child />
			}
		/>
	);
}

/**
 * Render the application
 * @returns JSX
 */
function App() {
	return (
		<Routes>
			{renderRoute("/error", ErrorWindow)}
			{renderRoute("/logs", LogsWindow)}
			{renderRoute("*", MainWindow)}
		</Routes>
	)
}

export default App;