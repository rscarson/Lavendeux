import { getExampleSample, getSampleHTML, getAllSamples, FormatterInstances } from '../index';

describe('getExampleSample', () => {
	it('contains all lines of the example', () => {
		expect(getExampleSample()).toMatch(/\n/);
	});
});

describe('getAllSamples', () => {
	it('contains the JSON sample objects', () => {
		expect(getAllSamples().length).toBeGreaterThan(1);
		expect(getAllSamples()[0].format).toMatch('lavendeux');
	});
});

describe('getSampleHTML', () => {
	it('contains the JSON sample objects', () => {
		expect(getSampleHTML()).toMatch('<span');
	});
});

describe('FormatterInstances', () => {
	it('has all language instances', () => {
		expect(FormatterInstances).toHaveProperty('lavendeux');
		expect(FormatterInstances).toHaveProperty('javascript');
	});

	describe('get', () => {
		it('gets recognized languages', () => {
			expect(FormatterInstances.get('javascript')).toBe(FormatterInstances.javascript);
		});
		it('defaults for unrecognized languages', () => {
			expect(FormatterInstances.get('floopidoober')).toBe(FormatterInstances.lavendeux);
		});
	});
});
