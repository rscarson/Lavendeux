import { mockIPC, clearMocks } from '@tauri-apps/api/mocks';
import { waitFor } from '@testing-library/react';
import { setTheme, registerThemeListener, updateTheme } from '../theme';

const crypto = require('crypto');

Object.defineProperty(global.self, 'crypto', {
	value: {
		getRandomValues: arr => crypto.randomBytes(arr.length),
	},
});

let addCalled = false;
let removeCalled = false;
const dummyElement = {
	classList: {
		add: () => addCalled = true,
		remove: () => removeCalled = true,
	},
};

describe('setTheme', () => {
	beforeEach(() => {
		addCalled = false;
		removeCalled = false;
	});

	it('should add the theme class', () => {
		setTheme(dummyElement, true);
		expect(addCalled).toEqual(true);
	});

	it('should remove the theme class', () => {
		setTheme(dummyElement, false);
		expect(removeCalled).toEqual(true);
	});
});

describe('registerThemeListener', () => {
	beforeEach(() => {
		addCalled = false;
		removeCalled = false;
	});

	it('add the theme on settings changes', () => {
		mockIPC((cmd, args) => {
			if (args.message.handler) {
				window[`_${args.message.handler}`]({ payload: { dark: true } });
				expect(addCalled).toEqual(true);
			} else if (cmd === 'get_settings') {
				return {
					dark: true,
				};
			}

			return undefined;
		});
		registerThemeListener(dummyElement);
		clearMocks();
	});
});

describe('updateTheme', () => {
	beforeEach(() => {
		addCalled = false;
		removeCalled = false;
	});

	afterEach(() => {
		clearMocks();
	});

	it('add the theme based on settings', async () => {
		mockIPC(cmd => {
			if (cmd === 'get_settings') {
				return { dark: true };
			}

			return undefined;
		});
		updateTheme(dummyElement);

		await waitFor(() => expect(addCalled).toEqual(true));
	});
});
