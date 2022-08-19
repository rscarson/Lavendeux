import React from "react";
import { setupMock, teardownMock, emitEvent, ipcEventCount } from "../../../mock.setup";
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';

import Extensions from '../extensions';

let extensions_reloaded = 0;
let extension_imported = 0;
let extensions_dir_opened = 0;
let extensions;
let container = null;
let root = null;
beforeEach(async () => {
    container = document.createElement("div");
    root = createRoot(container);

    extensions = [{
        "filename": "TESTFILE",
        "name": "TESTNAME",
        "author": "TESTAUTH",
        "version": "TESTVER",
        "functions": ["A", "B"],
        "decorators": ["C", "D"],
    }];
    
    setupMock((cmd, args) => {
        if (cmd == "reload_all_extensions") {
            extensions_reloaded++;
            return JSON.parse(JSON.stringify(extensions));
        } else if (cmd == "import_extension") {
            extension_imported++;
        }  else if (cmd == "open_extensions_dir") {
            extensions_dir_opened++;
        }
    });
    
    await act(async () => {
        root.render(<Extensions lang={{
            "extensionview_no_extensions": "TESTEMPTY"
        }} />);
    });

    extensions_reloaded = 0;
    extension_imported = 0;
    extensions_dir_opened = 0;
});

afterEach(() => {
    act(() => {
        root.unmount();
    });
    container.remove();
    container = null;
    teardownMock();
});

it("renders when empty", async () => {
    extensions = [];
    await act(async () => {
        emitEvent('extensions', extensions);
    });

    expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/);
});

it("renders extensions", async () => {
    expect(container.firstChild.innerHTML).toMatch(/TESTNAME/);
    expect(container.firstChild.innerHTML).toMatch(/TESTAUTH/);
    expect(container.firstChild.innerHTML).toMatch(/TESTVER/);
});

it("imports extensions", async () => {
    await act(async () => {
        container.querySelectorAll('button')[0].click();
    });

    await waitFor(() => expect(extension_imported).toBe(1));
});

it("reloads extensions", async () => {
    let backupExtensions = extensions;
    extensions = [];
    await act(async () => {
        emitEvent('extensions', extensions);
    });

    extensions = backupExtensions;
    await act(async () => {
        container.querySelectorAll('button')[1].click();
    });
    
    await waitFor(() => expect(extensions_reloaded).toBe(1));
    await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTNAME/));

    expect(container.firstChild.innerHTML).toMatch(/TESTNAME/);
    expect(container.firstChild.innerHTML).toMatch(/TESTAUTH/);
    expect(container.firstChild.innerHTML).toMatch(/TESTVER/);
});

it("opens the extensions directory", async () => {
    await act(async () => {
        container.querySelectorAll('button')[2].click();
    });

    await waitFor(() => expect(extensions_dir_opened).toBe(1));
});