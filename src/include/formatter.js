import sampleJSON from '../data/samples.json';

/**
 * Generic code formatter
 * Applies a set of formatting rules to an input string
 */
export class CodeFormatter {
	constructor() {
		this.rules = [];
	}

    /**
     * Replace a portion of a string at an index
     * @param {String} str Input String to apply replacement to
     * @param {int} len Length of the portion to replace
     * @param {int} index Start index to replace from
     * @param {String} replacement New string to put in
     * @returns str, with selected range replaced
     */
	static replaceAt(str, index, len, replacement) {
		return str.substring(0, index) + replacement + str.substring(index + len);
	}

    /**
     * Add a formatting rule to this formatter
     * @param {RegExp} pattern Regexp object for matching
     * @param {function(String) : String} replacement Handler for formatted replacement
     */
	addRule(pattern, replacement) {
		this.rules.push({
			"pattern": pattern,
			"handler": replacement
		});
	}

    /**
     * Apply a given rule to an input
     * @param {String} input String to apply corrections to
     * @param {String} search String to search in for matches
     * @param {RegExp} pattern Regexp object for matching
     * @param {function(String) : String} handler Handler for formatted replacement
     * @returns Formatted input
     */
	applyRule(target, search, rule) {
		let str = target;
		let remainder = search;
		let match; let regexp;
        
        // Get the regex ready
        if (rule.pattern !== null) {
            regexp = new RegExp(rule.pattern.source, rule.pattern.flags
                + (rule.pattern.global ? "" : "g")
                + (rule.pattern.multiline ? "" : "m")
            );
        } else {
            // Whole string rule
            return {
                result: rule.handler(target),
                remainder: rule.handler(target)
            };
        }

        // Regex rule
		while ((match = regexp.exec(remainder)) !== null) {
			let startIndex = regexp.lastIndex - match[0].length;
			let replacement = rule.handler(match[0]);
			let shadow = ''.padStart(replacement.length, ' ');
			
			str = CodeFormatter.replaceAt(str, startIndex, match[0].length, replacement);
			remainder = CodeFormatter.replaceAt(remainder, startIndex, match[0].length, shadow);
			regexp.lastIndex = startIndex + replacement.length;
		}

		return {
			result: str,
			remainder: remainder
		};
	}

    /**
     * Format an input string according to all formatter rules
     * @param {String} input String to apply rules to
     * @returns Formatted HTML code sample
     */
	format(input) {
		let str = input.trim();
		let remainder = str;
		for (const rule of this.rules) {
			let result = this.applyRule(str, remainder, rule);
			str = result.result;
			remainder = result.remainder;
		}

        return str;
	}
}

/**
 * Applies general formatting style and colouring
 */
export class HTMLFormatter extends CodeFormatter {
    SampleStyle = "background-color: #1f2937; color: #e5e7eb; padding: 1.1111111em 1.3333333em; line-height: 1.75;  margin-bottom: 2em; border-radius: .375rem;"
	Colours = {
        comment: "#33cc33",
        string: "#b1cbff",
        data: "#6699ff",
        radix: "#cc00cc",
        decorator: "#cc3300",
        function: "#ff9900",
    };

    /**
     * Wraps a sample in a span with a colour
     * @param {String} contents String to format
     * @param {String} colour Colour code
     */
    colouredSpan(contents, colour) {
        return `<span style="color:${colour}">${contents}</span>`;
    }

    /**
     * Wrap the whole sample
     * @param {String} sample Sample contents
     * @returns Formatted sample
     */
    wrapSample(sample) {
        return `<pre style="${this.SampleStyle}"><code>${sample}</code></pre>`;
    }
}

/**
 * Formatter for Lavendeux code
 */
export class LavendeuxFormatter extends HTMLFormatter {
	constructor() {
		super();

        // Comments
		this.addRule(/\/\/.*?$/, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/\/\*.*?\*\//s, (s) => this.colouredSpan(s, this.Colours.comment));

        // Functions and decorators
		this.addRule(/@[0-9A-Za-z_]+/, (s) => this.colouredSpan(s, this.Colours.decorator));
		this.addRule(/[0-9A-Za-z_]+\(/, (s) => `<span style="color:${this.Colours.function}">${s}`);
		this.addRule(/\)/, (s) => `${s}</span>`);

        // Values and strings
        this.addRule(/(?:"(?:\\.|[^"']|')*?")|(?:'(?:\\.|[^"']|")*?')/, (s) => this.colouredSpan(s, this.Colours.string));
		this.addRule(/(?<![0-9.,$])0[xXbBoO]?[a-zA-Z0-9]+/, (s) => this.colouredSpan(s, this.Colours.radix));
		this.addRule(/[$¥€£0-9A-Za-z._,]+(?:[eE]?[-+]?[0-7])?/, (s) => this.colouredSpan(s, this.Colours.data));

        this.addRule(null, (s) => this.wrapSample(s)); 
	}
}

/**
 * Formatter for JS code
 */
export class JavascriptFormatter extends HTMLFormatter {
	constructor() {
		super();
        
        // Comments
		this.addRule(/\/\/.*?$/, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/\/\*.*?\*\//s, (s) => this.colouredSpan(s, this.Colours.comment));

        // Functions
		this.addRule(/[0-9A-Za-z_]+\(/, (s) => `<span class="function">${s}`);
		this.addRule(/\)/, (s) => `${s}</span>`);
        
        // Values and strings
		this.addRule(/(?:"(?:\\.|[^"']|')*?")|(?:'(?:\\.|[^"']|")*?')/, (s) => this.colouredSpan(s, this.Colours.string));
		this.addRule(/\W(return|function)\W/, (s) => this.colouredSpan(s, this.Colours.decorator));
		this.addRule(/(?<![0-9.,$])0[xXbBoO]?[a-zA-Z0-9]+/, (s) => this.colouredSpan(s, this.Colours.radix));
		this.addRule(/[$¥€£0-9A-Za-z._,]+(?:[eE]?[-+]?[0-7])?/, (s) => this.colouredSpan(s, this.Colours.data));

        this.addRule(null, (s) => this.wrapSample(s)); 
	}
}

export const FormatterInstances = {
    "lavendeux": new LavendeuxFormatter(),
    "javascript": new JavascriptFormatter()
};

/**
 * A code sample object
 */
 export class CodeSample {
    constructor(name, format, description, code) {
        this.name = name;
        this.format = format;
        this.description = description;
        this.code = code;
    }

    /**
     * Retrieve a code sample from a json object
     * @param {Object} sample_obj 
     * @returns Code sample object
     */
    static from_json(sample_obj) {
        return new CodeSample(
            sample_obj.name, 
            sample_obj.format,
            sample_obj.description.join("\n"),
            sample_obj.code.join("\n")
        );
    }

    /**
     * Retrieve all code samples from a json object
     * @param {Object} source_obj 
     * @returns Array of code sample objects
     */
    static all_from_json(source_obj) {
        return source_obj.samples.map(e => this.from_json(e));
    }

    /**
     * Formats the code sample's preamble
     * @returns HTML string
     */
    description_to_html() {
        return `<p>${this.description.replaceAll("\n", "<br/>")}</p>`;
    }

    /**
     * Formats the code sample itself
     * @returns HTML string
     */
    code_to_html() {
        return FormatterInstances[this.format].format(this.code);
    }

    /**
     * Formats the code sample
     * @returns HTML string
     */
    to_html() {
        return `${this.description_to_html()}${this.code_to_html()}`;
    }
}

export const AllSamples = CodeSample.all_from_json(sampleJSON);
export const SampleHTML = AllSamples.map(s => s.to_html()).join("\n");
export const ExampleSample = sampleJSON.example.join("\n");