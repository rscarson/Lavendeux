import { CodeFormatter, HTMLFormatter, LavendeuxFormatter, JavascriptFormatter, CodeSample } from '../formatter';

describe('CodeFormatter', () => {
    let codeFormatter;
    beforeEach(() => {
        codeFormatter = new CodeFormatter();

    });
    
    describe('replaceAt', () => {
        it('replaces the given chunk', () => {
            let str = CodeFormatter.replaceAt('testtesttest', 4, 4, 'AAA');
            expect(str).toEqual('testAAAtest');
        })
    });

    describe('addRule', () => {
        it('registers the new rule with the formatter', () => {
            codeFormatter.addRule(/test/, s => `A${s}A`);
            expect(codeFormatter.rules.length).toEqual(1);
        })
    });
    
    describe('applyRule', () => {
        it('Applies the given rule to the whole string', () => {
            codeFormatter.addRule(/test/, s => `A${s}A`);
            expect(codeFormatter.applyRule('footesttesttestbar','footesttesttestbar', codeFormatter.rules[0])).toEqual({
                result: 'fooAtestAAtestAAtestAbar',
                remainder: 'foo                  bar'
            });
        })
    });
    
    describe('format', () => {
        it('Applies all rules to a string', () => {
            codeFormatter.addRule(/test/, s => `A${s}A`);
            codeFormatter.addRule(/foo/, s => `B${s}`);
            codeFormatter.addRule(/bar/, s => `${s}B`);
            expect(codeFormatter.format('footesttesttestbar')).toEqual(
                'BfooAtestAAtestAAtestAbarB'
            );
        })
    });
});

describe('HTMLFormatter', () => {
    let htmlFormatter;
    beforeEach(() => {
        htmlFormatter = new HTMLFormatter();
    });

    describe('colouredSpan', () => {
        it('wraps the entire string', () => {
            expect(htmlFormatter.colouredSpan('test', 'white')).toMatch(/^<span style=.*<\/span>/);
        })
    });

    describe('wrapSample', () => {
        it('wraps the entire string', () => {
           expect(htmlFormatter.wrapSample('test')).toMatch(/^<pre style=.*<\/pre>/);
        })
    });
});

describe('LavendeuxFormatter', () => {
    let lavendeuxFormatter;
    beforeEach(() => {
        lavendeuxFormatter = new LavendeuxFormatter();
    });

    it('wraps the entire sample', () => {
        let output = lavendeuxFormatter.format('foofoo');
        expect(output).toMatch(/<code>.+<\/code>/);
    });
    
    describe('Comments', () => {
        it('recognizes single line comments', () => {
            let output = lavendeuxFormatter.format('foofoo// test\nbar //stet');
            expect(output).toMatch(/foofoo<\/span><span.+\/\/ test/);
            expect(output).toMatch(/bar<\/span> <span.+\/\/stet/);
        });
        
        it('recognizes multiline comments', () => {
            let output = lavendeuxFormatter.format('foo /*test\ntest\ntest*/ bar');
            expect(output).toMatch(/>\/\*test\ntest\ntest\*\/<\//);
            expect(output).toMatch(/foo/);
            expect(output).toMatch(/bar/);
        });
    });

    describe('Values', () => {
        it('recognizes radix values', () => {
            let output = lavendeuxFormatter.format('0x5\n 0777\n 0b11\n 0o7\n0X5');
            expect(output).toMatch(/>0x5<\//);
            expect(output).toMatch(/>0777<\//);
            expect(output).toMatch(/>0b11<\//);
            expect(output).toMatch(/>0o7<\//);
            expect(output).toMatch(/>0X5<\//);
        });
        
        it('recognizes string values', () => {
            let output = lavendeuxFormatter.format('"test"\n\'test1\'\n"test2\\\"\"\n\'test3\\\'\'');
            expect(output).toMatch(/>"test"<\//);
            expect(output).toMatch(/>'test1'<\//);
            expect(output).toMatch(/>"test2\\""<\//);
            expect(output).toMatch(/>'test3\\''<\//);
        });
        
        it('recognizes numerical values', () => {
            let output = lavendeuxFormatter.format('5\n5.56\n.2e+3\n¥1,000.00\n5.6e+7\n.6E7\n.2e-3\n.6');
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
            let output = lavendeuxFormatter.format('foo1+bar1(2, 2)+bar2()+bar3(2)+foo2');
            expect(output).toMatch(/>bar1\(/);
            expect(output).toMatch(/>bar2\(/);
            expect(output).toMatch(/>bar3\(/);
            expect(output).toMatch(/<\//);
            expect(output).toMatch(/foo1/);
            expect(output).toMatch(/foo2/);
        });
        
        it('recognizes decorators', () => {
            let output = lavendeuxFormatter.format('foo @test3\nbar');
            expect(output).toMatch(/>@test3<\//);
            expect(output).toMatch(/foo/);
            expect(output).toMatch(/bar/);
        });
    });
});

describe('JavascriptFormatter', () => {
    let javascriptFormatter;
    beforeEach(() => {
        javascriptFormatter = new JavascriptFormatter();
    });

    it('wraps the entire sample', () => {
        let output = javascriptFormatter.format('foofoo');
        expect(output).toMatch(/<code>.+<\/code>/);
    });
    
    describe('Comments', () => {
        it('recognizes single line comments', () => {
            let output = javascriptFormatter.format('foofoo// test\nbar //stet');
            expect(output).toMatch(/foofoo<\/span><span.+\/\/ test/);
            expect(output).toMatch(/bar<\/span> <span.+\/\/stet/);
        });
        
        it('recognizes multiline comments', () => {
            let output = javascriptFormatter.format('foo /*test\ntest\ntest*/ bar');
            expect(output).toMatch(/>\/\*test\ntest\ntest\*\/<\//);
            expect(output).toMatch(/foo/);
            expect(output).toMatch(/bar/);
        });
    });

    describe('Values', () => {
        it('recognizes radix values', () => {
            let output = javascriptFormatter.format('0x5\n 0777\n 0b11\n 0o7\n0X5');
            expect(output).toMatch(/>0x5<\//);
            expect(output).toMatch(/>0777<\//);
            expect(output).toMatch(/>0b11<\//);
            expect(output).toMatch(/>0o7<\//);
            expect(output).toMatch(/>0X5<\//);
        });
        
        it('recognizes string values', () => {
            let output = javascriptFormatter.format('"test"\n\'test1\'\n"test2\\\"\"\n\'test3\\\'\'');
            expect(output).toMatch(/>"test"<\//);
            expect(output).toMatch(/>'test1'<\//);
            expect(output).toMatch(/>"test2\\""<\//);
            expect(output).toMatch(/>'test3\\''<\//);
        });
        
        it('recognizes numerical values', () => {
            let output = javascriptFormatter.format('5\n5.56\n.2e+3\n¥1,000.00\n5.6e+7\n.6E7\n.2e-3\n.6');
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
            let output = javascriptFormatter.format('foo1+bar1(2, 2)+bar2()+bar3(2)+foo2');
            expect(output).toMatch(/>bar1\(/);
            expect(output).toMatch(/>bar2\(/);
            expect(output).toMatch(/>bar3\(/);
            expect(output).toMatch(/<\//);
            expect(output).toMatch(/foo1/);
            expect(output).toMatch(/foo2/);
        });
    });
});

describe('CodeSample', () => {

    describe('from_json', () => {
        it('reads JSON input correctly', () => {
            let codeSample = CodeSample.from_json({
                "name": "test1",
                "description": [
                    "test2", "test3"
                ],
                "format": "lavendeux",
                "code": [
                    "test4", "test5"
                ]
            });

            expect(codeSample.name).toBe("test1");
            expect(codeSample.description).toBe("test2\ntest3");
            expect(codeSample.format).toBe("lavendeux");
            expect(codeSample.code).toBe("test4\ntest5");
        })
    });

    describe('all_from_json', () => {
        it('reads JSON input correctly', () => {
            let codeSamples = CodeSample.all_from_json({"samples": [
                {
                    "name": "test1",
                    "description": [
                        "test2", "test3"
                    ],
                    "format": "lavendeux",
                    "code": [
                        "test4", "test5"
                    ]
                },
                {
                    "name": "Btest1",
                    "description": [
                        "Btest2", "Btest3"
                    ],
                    "format": "lavendeux",
                    "code": [
                        "Btest4", "Btest5"
                    ]
                },
            ]});

            expect(codeSamples.length).toBe(2);
            expect(codeSamples[0].name).toBe("test1");
            expect(codeSamples[0].description).toBe("test2\ntest3");
            expect(codeSamples[0].format).toBe("lavendeux");
            expect(codeSamples[0].code).toBe("test4\ntest5");
            
            expect(codeSamples[1].name).toBe("Btest1");
            expect(codeSamples[1].description).toBe("Btest2\nBtest3");
            expect(codeSamples[1].format).toBe("lavendeux");
            expect(codeSamples[1].code).toBe("Btest4\nBtest5");
        })
    });

    describe('description_to_html', () => {
        let codeSample;
        beforeEach(() => {
            codeSample = CodeSample.from_json({
                "name": "test1",
                "description": [ "test2", "test3" ],
                "format": "lavendeux",
                "code": [ "test4", "test5" ]
            });
        });

        it('wraps the snippet', () => {
            expect(codeSample.description_to_html()).toMatch(/<p>.*<\/p>/);
        });
        
        it('replaces line breaks', () => {
            expect(codeSample.description_to_html()).not.toMatch(/\n/);
            expect(codeSample.description_to_html()).toMatch(/<br\/>/);
        });
    });

    describe('code_to_html', () => {
        let codeSample;
        beforeEach(() => {
            codeSample = CodeSample.from_json({
                "name": "test1",
                "description": [ "test2", "test3" ],
                "format": "lavendeux",
                "code": [ "test4", "test5" ]
            });
        });

        it('calls the configured formatter', () => {
            expect(codeSample.code_to_html()).toMatch(/<span/);

            codeSample.format = "javascript";
            expect(codeSample.code_to_html()).toMatch(/<span/);
        });
    });

    describe('to_html', () => {
        let codeSample;
        beforeEach(() => {
            codeSample = CodeSample.from_json({
                "name": "test1",
                "description": [ "test2", "test3" ],
                "format": "lavendeux",
                "code": [ "test4", "test5" ]
            });
        });

        it('contains all parts of the snippet', () => {
            expect(codeSample.to_html()).toMatch(/test2/);
            expect(codeSample.to_html()).toMatch(/test5/);
        });
    });
});