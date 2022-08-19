import React from "react";
import { setupMock, teardownMock, emitEvent, ipcEventCount } from "../../../mock.setup";
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';

import Help from '../help';

let container = null;
let root = null;
beforeEach(async () => {
    container = document.createElement("div");
    root = createRoot(container);

    setupMock((cmd, args) => {
        if (args.message && args.message.cmd == "getAppName") {
            return 'TESTNAME';
        } else if (args.message && args.message.cmd == "getAppVersion") {
            return 'TESTVER';
        } else if (cmd == 'format_shortcut') {
            return 'Shift+A';
        }
    });
    
    await act(async () => {
        root.render(<Help lang={{
            'historyview_getting_started_highlight': 'HIGHLIGHT',
            'historyview_getting_started_copy': 'COPY',
        }} />);
    });

    await waitFor(() => expect(ipcEventCount('listen.settings')).toBe(1));
    await act(async () => {
        emitEvent('settings', {
            'autopaste': false
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

it("renders the title", () => {
    expect(container.firstChild.innerHTML).toMatch(/TESTNAME/);
    expect(container.firstChild.innerHTML).toMatch(/TESTVER/);
});

it("renders the code sample", () => {
    expect(container.firstChild.innerHTML).toMatch(/textarea/);
    expect(container.firstChild.innerHTML).toMatch(/Shift\+A/);
    expect(container.firstChild.innerHTML).toMatch(/COPY/);
});

it("renders the samples", () => {
    expect(container.firstChild.innerHTML).toMatch(/ceil\(/);
});