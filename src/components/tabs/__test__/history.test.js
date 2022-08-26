import React from 'react';
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';
import { setupMock, teardownMock, emitEvent, ipcEventCount } from '../../../mock.setup';

import History from '../history';

let history = [];
let container = null;
let root = null;

beforeEach(async () => {
	container = document.createElement('div');
	root = createRoot(container);

	history = [
		{
			expression: 'EXPRESSION',
			result: { Ok: 'RESULT' },
		},
		{
			expression: 'EXPRESSION',
			result: { Err: 'RESULT' },
		},
	];

	setupMock(cmd => {
		if (cmd === 'clear_history') {
			history = [];
			emitEvent('history', history);
			return {};
		}

		return undefined;
	});

	await act(async () => {
		root.render(<History lang={{
			historyview_no_history: 'TESTEMPTY',
		}}
		/>);
	});

	await waitFor(() => expect(ipcEventCount('listen.history')).toBe(1));
	await act(async () => {
		emitEvent('history', history);
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

it('renders history', async () => {
	expect(container.firstChild.innerHTML).toMatch(/danger/);
	expect(container.firstChild.innerHTML).toMatch(/secondary/);
	expect(container.firstChild.innerHTML).toMatch(/EXPRESSION/);
});

it('renders empty history', async () => {
	history = [];
	await act(async () => {
		emitEvent('history', history);
	});

	expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/);
});

it('clears the history', async () => {
	await act(async () => {
		container.querySelector('button').click();
	});

	await waitFor(() => expect(ipcEventCount('emit.history')).toBe(2));
	await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/));
});
