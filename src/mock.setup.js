import { mockIPC, clearMocks } from '@tauri-apps/api/mocks';
import { randomFillSync } from 'crypto';
import { emit } from '@tauri-apps/api/event';

let callCount;
let eventCount;
let eventHandlers;
let eventHandlerIDs;

export function setupMock(handler) {
	callCount = {};
	eventCount = {};
	eventHandlers = {};
	eventHandlerIDs = {};

	window.alert = jest.fn(e => console.log(e));
	global.self.crypto || Object.defineProperty(global.self, 'crypto', {
		value: {
			getRandomValues: buffer => randomFillSync(buffer),
		},
	});

	mockIPC((cmd, args) => {
		const v = handler(cmd, args);
		if (v !== undefined) return v;

		if (args.message) {
			const eCmd = `${args.message.cmd}.${args.message.event}`;
			eventCount[eCmd] = (eventCount[eCmd] === undefined)
				? 1
				: eventCount[eCmd] + 1;

			switch (args.message.cmd) {
			case 'emit':
				try {
					args.message.payload = JSON.parse(args.message.payload);
				} catch (e) { }
				eventHandlers[args.message.event] && eventHandlers[args.message.event](args.message);
				return args.message;
			case 'listen':
				eventHandlers[args.message.event] = window[`_${args.message.handler}`];
				eventHandlerIDs[args.message.event] = args.message.handler;
			}
		} else {
			callCount[cmd] = (callCount[cmd] === undefined)
				? 1
				: callCount[cmd] + 1;

			switch (cmd) {
			case 'get_settings':
			case 'get_language_strings':
				return {};
			}
		}

		return undefined;
	});
}

export function ipcInvokeCount(cmd) {
	return callCount[cmd] || 0;
}

export function ipcEventCount(event) {
	return eventCount[event] || 0;
}

export function teardownMock() {
	delete global.self.crypto;
	window.alert.mockClear();
	clearMocks();

	// Delete dangling handlers
	Object.values(eventHandlerIDs).map(e => delete window[e]);
}

export function emitEvent(name, payload) {
	emit(name, payload);
}
