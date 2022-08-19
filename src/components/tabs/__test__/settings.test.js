import React from "react";
import { setupMock, teardownMock, emitEvent, ipcEventCount } from "../../../mock.setup";
import { createRoot } from 'react-dom/client';
import { render, act, fireEvent, waitFor } from '@testing-library/react';


import Settings from '../settings';

let settings = {
    'auto_paste': true,
    'silent_errors': true,
    'autostart': true,
    'dark': true,
    'shortcut': 'CmdOrCtrl+Shift+Space',
    'extension_dir': 'TEST'
};

let container = null;
let root = null;
beforeEach(async () => {
    container = document.createElement("div");
    root = createRoot(container);

    settings = {
        'auto_paste': true,
        'silent_errors': true,
        'autostart': true,
        'dark': true,
        'shortcut': 'Shift+A',
        'extension_dir': 'TEST'
    };
    
    setupMock((cmd, args) => {
        switch(cmd) {
            case "get_settings": 
                return JSON.parse(JSON.stringify(settings));
            case "update_settings":
                settings = args.settings;
                return;
        }
    });
    
    await act(async () => {
        root.render(<Settings lang={{}} />);
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

describe("renderClipboardMode", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("input[name='autoPaste']").length).toBe(2);
        expect(container.querySelector('#autopasteOn').checked).toBe(true);
    });
    
    it("updates the settings", () => {
        act(() => {
            container.querySelector('#autopasteOff').click();
            container.querySelector('button').click();
        });
        expect(settings.auto_paste).toBe(false);
        
        act(() => {
            container.querySelector('#autopasteOn').click();
            container.querySelector('button').click();
        });
        expect(settings.auto_paste).toBe(true);
    });
})

describe("renderErrorMode", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("input[name='silent_errors']").length).toBe(2);
        expect(container.querySelector('#silent_errorsOn').checked).toBe(true);
    });
    
    it("updates the settings", () => {
        act(() => {
            container.querySelector('#silent_errorsOff').click();
            container.querySelector('button').click();
        });
        expect(settings.silent_errors).toBe(false);
        
        act(() => {
            container.querySelector('#silent_errorsOn').click();
            container.querySelector('button').click();
        });
        expect(settings.silent_errors).toBe(true);
    });
})

describe("renderAutostart", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("input[name='autostart']").length).toBe(2);
        expect(container.querySelector('#autostartOn').checked).toBe(true);
    });
    
    it("updates the settings", () => {
        act(() => {
            container.querySelector('#autostartOff').click();
            container.querySelector('button').click();
        });
        expect(settings.autostart).toBe(false);
        
        act(() => {
            container.querySelector('#autostartOn').click();
            container.querySelector('button').click();
        });
        expect(settings.autostart).toBe(true);
    });
})

describe("renderTheme", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("input[name='dark']").length).toBe(2);
        expect(container.querySelector('#darkOn').checked).toBe(true);
    });
    
    it("updates the settings", () => {
        act(() => {
            container.querySelector('#darkOff').click();
            container.querySelector('button').click();
        });
        expect(settings.dark).toBe(false);
        
        act(() => {
            container.querySelector('#darkOn').click();
            container.querySelector('button').click();
        });
        expect(settings.dark).toBe(true);
    });
})

describe("renderKeyboardShortcut", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("#shortcut_mod").length).toBe(1);
        expect(container.querySelectorAll("#shortcut_key").length).toBe(1);
        expect(container.querySelector('#shortcut_mod').value).toBe("Shift");
        expect(container.querySelector('#shortcut_key').value).toBe("A");
    });
    
    it("updates the settings", () => {
        act(() => {
            fireEvent.change(container.querySelector('#shortcut_mod'), {target: {value: 'CmdOrCtrl'}})
            fireEvent.change(container.querySelector('#shortcut_key'), {target: {value: 'Space'}})
            container.querySelector('button').click();
        });
        expect(settings.shortcut).toBe("CmdOrCtrl+Space");
    });
})

describe("renderExtensionDir", () => {
    it("renders the form correctly", () => {
        expect(container.querySelectorAll("#extension_dir").length).toBe(1);
        expect(container.querySelector('#extension_dir').value).toBe("TEST");
    });
    
    it("updates the settings", () => {
        act(() => {
            fireEvent.change(container.querySelector('#extension_dir'), {target: {value: 'FOOBAR'}})
            container.querySelector('button').click();
        });
        expect(settings.extension_dir).toBe("FOOBAR");
    });
})