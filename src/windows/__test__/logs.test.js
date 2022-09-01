import React from 'react';
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';
import { setupMock, teardownMock, ipcEventCount, emitEvent } from '../../mock.setup';

import Logs from '../logs';

let dirOpened = 0;
let logs = [];
let container = null;
let root = null;
beforeEach(async () => {
	container = document.createElement('div');
	root = createRoot(container);

	logs = [
		{
			timestamp: 'TESTTIME',
			level: 'TESTLEVEL',
			text: 'tESTTEXT',
		},
	];

	setupMock(cmd => {
		if (cmd === 'get_language_strings') {
			return {
				logview_empty: 'TESTEMPTY',
			};
		} if (cmd === 'get_logs') {
			return JSON.parse(JSON.stringify(logs));
		} if (cmd === 'open_logs_dir') {
			dirOpened++;
		} else if (cmd === 'clear_logs') {
			logs = [];
			emitEvent('logs', logs);
		}

		return undefined;
	});

	await act(async () => {
		root.render(<Logs />);
	});

	await waitFor(() => expect(ipcEventCount('listen.logs')).toBe(1));
	await act(async () => {
		emitEvent('logs', logs);
	});
});

afterEach(() => {
	act(() => {
		root.unmount();
	});
	container.remove();
	container = null;
	teardownMock();
});

it('renders log entries', async () => {
	expect(container.firstChild.innerHTML).toMatch(/TESTLEVEL/);
});

it('renders when empty entries', async () => {
	logs = [];
	await act(async () => {
		emitEvent('logs', logs);
	});

	await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/));
});

it('refreshes log entries', async () => {
	expect(container.firstChild.innerHTML).toMatch(/TESTLEVEL/);
	expect(container.firstChild.innerHTML).not.toMatch(/TESTNEXTLEVEL/);

	logs.push({
		timestamp: 'TESTNEXTTIME',
		level: 'TESTNEXTLEVEL',
		text: 'tESTNEXTTEXT',
	})

	await act(async () => {
		container.querySelectorAll('button')[0].click();
	});

	expect(container.firstChild.innerHTML).toMatch(/TESTLEVEL/);
	expect(container.firstChild.innerHTML).toMatch(/TESTNEXTLEVEL/);
});

it('clears log entries', async () => {
	await act(async () => {
		container.querySelectorAll('button')[1].click();
	});

	await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/));
});

it('opens the log directory', async () => {
	await act(async () => {
		container.querySelectorAll('button')[2].click();
	});

	await waitFor(() => expect(dirOpened).toBe(1));
});
