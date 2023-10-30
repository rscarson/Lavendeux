import { LavendeuxFormatter } from '../index';

describe('LavendeuxFormatter', () => {
	let lavendeuxFormatter;
	beforeEach(() => {
		lavendeuxFormatter = new LavendeuxFormatter();
	});

	it('wraps the entire sample', () => {
		const output = lavendeuxFormatter.format('foofoo');
		expect(output).toMatch(/<pre.+<\/pre>/);
	});

	describe('Comments', () => {
		it('recognizes single line comments', () => {
			const output = lavendeuxFormatter.format('foofoo// test\nbar //stet');
			expect(output).toMatch(/foofoo<\/span><span.+\/\/ test/);
			expect(output).toMatch(/bar<\/span> <span.+\/\/stet/);
		});

		it('recognizes multiline comments', () => {
			const output = lavendeuxFormatter.format('foo /*test\ntest\ntest*/ bar');
			expect(output).toMatch(/>\/\*test\ntest\ntest\*\/<\//);
			expect(output).toMatch(/foo/);
			expect(output).toMatch(/bar/);
		});
	});

	describe('Values', () => {
		it('recognizes radix values', () => {
			const output = lavendeuxFormatter.format('0x5\n 0777\n 0b11\n 0o7\n0X5');
			expect(output).toMatch(/>0x5<\//);
			expect(output).toMatch(/>0777<\//);
			expect(output).toMatch(/>0b11<\//);
			expect(output).toMatch(/>0o7<\//);
			expect(output).toMatch(/>0X5<\//);
		});

		it('recognizes string values', () => {
			const output = lavendeuxFormatter.format('"test"\n\'test1\'\n"test2\\""\n\'test3\\\'\'');
			expect(output).toMatch(/>"test"<\//);
			expect(output).toMatch(/>'test1'<\//);
			expect(output).toMatch(/>"test2\\""<\//);
			expect(output).toMatch(/>'test3\\''<\//);
		});

		it('recognizes numerical values', () => {
			const output = lavendeuxFormatter.format('5\n5.56\n.2e+3\n¥1,000.00\n5.6e+7\n.6E7\n.2e-3\n.6');
			expect(output).toMatch(/>5<\//);
			expect(output).toMatch(/>5.56<\//);
			expect(output).toMatch(/>.2e\+3<\//);
			expect(output).toMatch(/>¥1,000.00<\//);
			expect(output).toMatch(/>5.6e\+7<\//);
			expect(output).toMatch(/>.6E7<\//);
			expect(output).toMatch(/>.2e-3<\//);
			expect(output).toMatch(/>.6<\//);
		});
	});

	describe('Syntax', () => {
		it('recognizes functions', () => {
			const output = lavendeuxFormatter.format('foo1+bar1(2, 2)+bar2()+bar3(2)+foo2');
			expect(output).toMatch(/>bar1\(/);
			expect(output).toMatch(/>bar2\(/);
			expect(output).toMatch(/>bar3\(/);
			expect(output).toMatch(/<\//);
			expect(output).toMatch(/foo1/);
			expect(output).toMatch(/foo2/);
		});

		it('recognizes decorators', () => {
			const output = lavendeuxFormatter.format('foo @test3\nbar');
			expect(output).toMatch(/>@test3<\//);
			expect(output).toMatch(/foo/);
			expect(output).toMatch(/bar/);
		});
	});
});
