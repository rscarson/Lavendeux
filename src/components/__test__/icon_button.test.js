import React from 'react';
import { createRoot } from 'react-dom/client';
import { act } from '@testing-library/react';

import { IconButton } from '../icon_button';

let container = null;
let root = null;
beforeEach(() => {
	container = document.createElement('div');
	root = createRoot(container);
});

afterEach(() => {
	act(() => {
		root.unmount();
	});
	container.remove();
	container = null;
});

it('renders without a variant', () => {
	act(() => {
		root.render(<IconButton title="test" />);
	});
	expect(container.firstChild.classList.contains('btn-outline-secondary')).toBe(true);
});

it('renders without an onClick handler', () => {
	act(() => {
		root.render(<IconButton title="test" />);
	});
	container.firstChild.click();
	expect(container.firstChild.classList.contains('btn-outline-secondary')).toBe(true);
});

it('renders with an onClick handler', () => {
	const mockCallBack = jest.fn();

	act(() => {
		root.render(<IconButton title="test" onClick={mockCallBack} />);
	});
	container.firstChild.click();
	expect(mockCallBack.mock.calls.length).toEqual(1);
	expect(container.firstChild.classList.contains('btn-outline-secondary')).toBe(true);
});
