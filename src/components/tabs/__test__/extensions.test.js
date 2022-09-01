import React from 'react';
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';
import { setupMock, teardownMock, emitEvent } from '../../../mock.setup';

import Extensions from '../extensions';

let extensionsReloaded = 0;
let extensionImported = 0;
let extensionsDirOpened = 0;
let extensions;
let container = null;
let root = null;
beforeEach(async () => {
	container = document.createElement('div');
	root = createRoot(container);

	extensions = [{
		filename: 'TESTFILE',
		name: 'TESTNAME',
		author: 'TESTAUTH',
		version: 'TESTVER',
		functions: ['A', 'B'],
		decorators: ['C', 'D'],
	}];

	setupMock(cmd => {
		if (cmd === 'reload_extensions') {
			extensionsReloaded++;
			return JSON.parse(JSON.stringify(extensions));
		} if (cmd === 'import_extension') {
			extensionImported++;
		} else if (cmd === 'open_extensions_dir') {
			extensionsDirOpened++;
		}

		return undefined;
	});

	await act(async () => {
		root.render(<Extensions lang={{
			extensionview_no_extensions: 'TESTEMPTY',
		}}
		/>);
	});

	extensionsReloaded = 0;
	extensionImported = 0;
	extensionsDirOpened = 0;
});

afterEach(() => {
	act(() => {
		root.unmount();
	});
	container.remove();
	container = null;
	teardownMock();
});

it('renders when empty', async () => {
	extensions = [];
	await act(async () => {
		emitEvent('extensions', extensions);
	});

	expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/);
});

it('renders extensions', async () => {
	expect(container.firstChild.innerHTML).toMatch(/TESTNAME/);
	expect(container.firstChild.innerHTML).toMatch(/TESTAUTH/);
	expect(container.firstChild.innerHTML).toMatch(/TESTVER/);
});

it('imports extensions', async () => {
	await act(async () => {
		container.querySelectorAll('button')[0].click();
	});

	await waitFor(() => expect(extensionImported).toBe(1));
});

it('reloads extensions', async () => {
	const backupExtensions = extensions;
	extensions = [];
	await act(async () => {
		emitEvent('extensions', extensions);
	});

	extensions = backupExtensions;
	await act(async () => {
		container.querySelectorAll('button')[1].click();
	});

	await waitFor(() => expect(extensionsReloaded).toBe(1));
	await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTNAME/));

	expect(container.firstChild.innerHTML).toMatch(/TESTNAME/);
	expect(container.firstChild.innerHTML).toMatch(/TESTAUTH/);
	expect(container.firstChild.innerHTML).toMatch(/TESTVER/);
});

it('opens the extensions directory', async () => {
	await act(async () => {
		container.querySelectorAll('button')[2].click();
	});

	await waitFor(() => expect(extensionsDirOpened).toBe(1));
});
