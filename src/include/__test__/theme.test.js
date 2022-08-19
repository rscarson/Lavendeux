import { mockIPC, clearMocks } from "@tauri-apps/api/mocks"
import { setTheme, registerThemeListener, updateTheme } from '../theme';

const crypto = require('crypto');
Object.defineProperty(global.self, 'crypto', {
	value: {
		getRandomValues: (arr) => crypto.randomBytes(arr.length)
	}
});

let add_called = false;
let remove_called = false;
const dummy_element = {
    classList: {
        add: (s) => add_called = true,
        remove: (s) => remove_called = true,
    }
};

describe('setTheme', () => {
    beforeEach(() => {
        add_called = false;
        remove_called = false;
    });

    it('should add the theme class', () => {
        setTheme(dummy_element, true);
        expect(add_called).toEqual(true);
    });

    it('should remove the theme class', () => {
        setTheme(dummy_element, false);
        expect(remove_called).toEqual(true);
    });
});

describe('registerThemeListener', () => {
    beforeEach(() => {
        add_called = false;
        remove_called = false;
    });

    it('add the theme on settings changes', () => {
        mockIPC((cmd, args) => {
            if (args.message.handler) {
                window[`_${args.message.handler}`]({payload: {dark: true}});
                expect(add_called).toEqual(true);
            } else if (cmd == 'get_settings') {
                return {
                    dark: true
                }
            }
        });
        registerThemeListener(dummy_element);
        clearMocks();
    });
});

describe('updateTheme', () => {
    beforeEach(() => {
        add_called = false;
        remove_called = false;
    });

    it('add the theme based on settings', function(done) {
        mockIPC((cmd, args) => {
            if (cmd == 'get_settings') {
                return { dark: true }
            }
        });
        updateTheme(dummy_element);
        setTimeout(() => {
            expect(add_called).toEqual(true);
            clearMocks();
            done();
        }, 50);
    });
});