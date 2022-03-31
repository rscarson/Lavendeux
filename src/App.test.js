import { render, screen } from '@testing-library/react';
import {BrowserRouter as Router} from "react-router-dom"
import App from './App';

const crypto = require('crypto');
Object.defineProperty(global.self, 'crypto', {
	value: {
		getRandomValues: (arr) => crypto.randomBytes(arr.length)
	}
});

window.__TAURI_IPC__ = () => {};

test('renders learn react link', () => {
	render(<Router fluid>
		<App />
	</Router>);

	expect(screen.getAllByText(/History/i).length).toBeGreaterThan(0);
	expect(screen.getAllByText(/Extensions/i).length).toBeGreaterThan(0);
	expect(screen.getAllByText(/Settings/i).length).toBeGreaterThan(0);
	expect(screen.getAllByText(/Help/i).length).toBeGreaterThan(0);
});
