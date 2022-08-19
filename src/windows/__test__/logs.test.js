import React from "react";
import { setupMock, teardownMock, ipcEventCount, emitEvent } from "../../mock.setup";
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';

import Logs from '../logs';

let dir_opened = 0;
let logs = [];
let container = null;
let root = null;
beforeEach(async () => {
    container = document.createElement("div");
    root = createRoot(container);

    logs = [
        {
            'timestamp': 'TESTTIME',
            'level': 'TESTLEVEL',
            'text': 'tESTTEXT'
        }   
    ];
    
    setupMock((cmd, args) => {
        if (cmd == "get_lang") {
            return {
                "logview_empty": "TESTEMPTY"
            };
        } else if (cmd == "get_logs") {
            return JSON.parse(JSON.stringify(logs));
        } else if (cmd == "open_logs_dir") {
            dir_opened++;
        } else if (cmd == "clear_logs") {
            logs = [];
            emitEvent("logs", logs);
        }
    });

    await act(async () => {
        root.render(<Logs />);
    });

    await waitFor(() => expect(ipcEventCount('listen.logs')).toBe(1));
    await act(async () => {
        emitEvent('logs', logs);
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

it("renders log entries", async () => {
    expect(container.firstChild.innerHTML).toMatch(/TESTLEVEL/);
});

it("renders when empty entries", async () => {
    logs = [];
    await act(async () => {
        emitEvent('logs', logs);
    });

    await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/));
});

it("clears log entries", async () => {    
    await act(async () => {
        container.querySelectorAll('button')[0].click();
    });

    await waitFor(() => expect(container.firstChild.innerHTML).toMatch(/TESTEMPTY/));
});

it("opens the log directory", async () => {    
    await act(async () => {
        container.querySelectorAll('button')[1].click();
    });

    await waitFor(() => expect(dir_opened).toBe(1));
});