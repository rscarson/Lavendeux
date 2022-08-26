import { Formatter } from '../index';

describe('Formatter', () => {
	let formatter;
	beforeEach(() => {
		formatter = new Formatter();
	});

	describe('tagWrap', () => {
		it('wraps the entire string', () => {
			expect(formatter.tagWrap('test', 'tag', 'white')).toMatch(/^<tag class='white'>test<\/tag>/);
			expect(formatter.tagWrap('test', 'tag', 'white', true)).toMatch(/^<tag class='white'>test/);
		});
	});

	describe('span', () => {
		it('wraps the entire string', () => {
			expect(formatter.span('test', 'white')).toMatch(/^<span class='white'>test<\/span>/);
		});
	});

	describe('outer', () => {
		it('replaces the given chunk', () => {
			expect(formatter.outer('test')).toMatch(/<pre class='formatted-sample'><code class=''>test<\/code><\/pre>/);
		});
	});

	describe('replaceAt', () => {
		it('replaces the given chunk', () => {
			const str = formatter.replaceAt('testtesttest', 4, 4, 'AAA');
			expect(str).toEqual('testAAAtest');
		});
	});

	describe('addRule', () => {
		it('registers the new rule with the formatter', () => {
			formatter.addRule(/test/, s => `A${s}A`);
			expect(formatter.rules.length).toEqual(1);
		});
	});

	describe('applyRule', () => {
		it('Applies the given rule to the whole string', () => {
			formatter.addRule(/test/, s => `A${s}A`);
			expect(formatter.applyRule('footesttesttestbar', 'footesttesttestbar', formatter.rules[0])).toEqual({
				result: 'fooAtestAAtestAAtestAbar',
				remainder: 'foo                  bar',
			});
		});
	});

	describe('format', () => {
		it('Applies all rules to a string', () => {
			formatter.addRule(/test/, s => `A${s}A`);
			formatter.addRule(/foo/, s => `B${s}`);
			formatter.addRule(/bar/, s => `${s}B`);
			expect(formatter.format('footesttesttestbar')).toEqual(
				'BfooAtestAAtestAAtestAbarB',
			);
		});
	});
});
