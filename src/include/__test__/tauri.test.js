import { mockIPC, clearMocks } from '@tauri-apps/api/mocks';
import { run } from '../tauri';

const crypto = require('crypto');

Object.defineProperty(global.self, 'crypto', {
	value: {
		getRandomValues: arr => crypto.randomBytes(arr.length),
	},
});

describe('run', () => {
	it('invoke the selected handler', () => {
		mockIPC((cmd, args) => {
			expect(cmd).toEqual('test');
			expect(args).toEqual({ test: true });
		});
		run('test', { test: true });
		clearMocks();
	});
});
