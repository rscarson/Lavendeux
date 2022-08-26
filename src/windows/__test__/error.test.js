import React from 'react';
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';
import { setupMock, teardownMock, ipcEventCount, emitEvent } from '../../mock.setup';

import Error from '../error';

let container = null;
let root = null;
beforeEach(async () => {
	window.alert = jest.fn(e => console.log(e));
	container = document.createElement('div');
	root = createRoot(container);

	setupMock(() => { });

	await act(async () => {
		root.render(<Error />);
	});

	await waitFor(() => expect(ipcEventCount('listen.message')).toBe(1));
	await act(async () => {
		emitEvent('message', {
			msg: 'TESTMSG',
			title: 'TESTTITLE',
			variant: 'danger',
		});
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

it('renders messages', async () => {
	await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTMSG/));
	expect(container.firstChild.innerHTML).toMatch(/TESTTITLE/);
});
