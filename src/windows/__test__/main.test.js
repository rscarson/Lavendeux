import React from "react";
import { setupMock, teardownMock, ipcEventCount, emitEvent } from "../../mock.setup";
import { createRoot } from 'react-dom/client';
import { act, waitFor } from '@testing-library/react';

jest.mock("../../components/tabs/history", () => () => {
    return <mock-modal data-testid="modal"/>;
});
jest.mock("../../components/tabs/help", () => () => {
    return <mock-modal data-testid="modal"/>;
});
jest.mock("../../components/tabs/settings", () => () => {
    return <mock-modal data-testid="modal"/>;
});
jest.mock("../../components/tabs/extensions", () => () => {
    return <mock-modal data-testid="modal"/>;
});

import MainWindow from '../main';

let ready = false;
let container = null;
let root = null;
beforeEach(async () => {
    container = document.createElement("div");
    root = createRoot(container);
    
    setupMock((cmd, args) => {
        if (args.message && args.message.event == "ready") {
            ready = true;
        }
    });
    
    await act(async () => {
        root.render(<MainWindow />);
    });
});

afterEach(() => {
    act(() => {
        root.unmount();
    });
    container.remove();
    container = null;
    ready = false;
    teardownMock();
});

it("renders", async () => {
    await act(async () => {
        root.render(<MainWindow />);
    });

    await waitFor(() => expect(ready).toBe(true));
});