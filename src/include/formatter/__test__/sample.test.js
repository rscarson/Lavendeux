import { JSONSample, LavendeuxFormatter } from '../index';

const lavendeuxFormatter = new LavendeuxFormatter();

describe('JSONSample', () => {
	describe('fromJSON', () => {
		it('reads JSON input correctly', () => {
			const jsonSample = JSONSample.fromJSON({
				name: 'test1',
				description: [
					'test2', 'test3',
				],
				format: 'lavendeux',
				code: [
					'test4', 'test5',
				],
			});

			expect(jsonSample.name).toBe('test1');
			expect(jsonSample.description).toBe('test2\ntest3');
			expect(jsonSample.format).toBe('lavendeux');
			expect(jsonSample.code).toBe('test4\ntest5');
		});
	});

	describe('allFromJSON', () => {
		it('reads JSON input correctly', () => {
			const jsonSamples = JSONSample.allFromJSON({ samples: [
				{
					name: 'test1',
					description: [
						'test2', 'test3',
					],
					format: 'lavendeux',
					code: [
						'test4', 'test5',
					],
				},
				{
					name: 'Btest1',
					description: [
						'Btest2', 'Btest3',
					],
					format: 'lavendeux',
					code: [
						'Btest4', 'Btest5',
					],
				},
			] });

			expect(jsonSamples.length).toBe(2);
			expect(jsonSamples[0].name).toBe('test1');
			expect(jsonSamples[0].description).toBe('test2\ntest3');
			expect(jsonSamples[0].format).toBe('lavendeux');
			expect(jsonSamples[0].code).toBe('test4\ntest5');

			expect(jsonSamples[1].name).toBe('Btest1');
			expect(jsonSamples[1].description).toBe('Btest2\nBtest3');
			expect(jsonSamples[1].format).toBe('lavendeux');
			expect(jsonSamples[1].code).toBe('Btest4\nBtest5');
		});
	});

	describe('descriptionToHTML', () => {
		let jsonSample;
		beforeEach(() => {
			jsonSample = JSONSample.fromJSON({
				name: 'test1',
				description: ['test2', 'test3'],
				format: 'lavendeux',
				code: ['test4', 'test5'],
			});
		});

		it('wraps the snippet', () => {
			expect(jsonSample.descriptionToHTML(lavendeuxFormatter)).toMatch(/<p.*<\/p>/);
		});

		it('replaces line breaks', () => {
			expect(jsonSample.descriptionToHTML(lavendeuxFormatter)).not.toMatch(/\n/);
			expect(jsonSample.descriptionToHTML(lavendeuxFormatter)).toMatch(/<br\/>/);
		});
	});

	describe('codeToHTML', () => {
		let jsonSample;
		beforeEach(() => {
			jsonSample = JSONSample.fromJSON({
				name: 'test1',
				description: ['test2', 'test3'],
				format: 'lavendeux',
				code: ['test4', 'test5'],
			});
		});

		it('calls the configured formatter', () => {
			expect(jsonSample.codeToHTML(lavendeuxFormatter)).toMatch(/<span/);

			jsonSample.format = 'javascript';
			expect(jsonSample.codeToHTML(lavendeuxFormatter)).toMatch(/<span/);
		});
	});

	describe('toHTML', () => {
		let jsonSample;
		beforeEach(() => {
			jsonSample = JSONSample.fromJSON({
				name: 'test1',
				description: ['test2', 'test3'],
				format: 'lavendeux',
				code: ['test4', 'test5'],
			});
		});

		it('contains all parts of the snippet', () => {
			expect(jsonSample.toHTML(lavendeuxFormatter)).toMatch(/test2/);
			expect(jsonSample.toHTML(lavendeuxFormatter)).toMatch(/test5/);
		});
	});
});
