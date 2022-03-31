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
	replaceAt(str, index, len, replacement) {
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
	applyRule(input, search, pattern, handler) {
		let str = input;
		let remainder = search;
		let match; let regexp = pattern;

        // Whole string rule
        if (regexp === null) {
            return {
                result: handler(input),
                remainder: handler(input)
            };
        }

        // Regex rule
		while ((match = regexp.exec(remainder)) !== null) {
			let startIndex = regexp.lastIndex - match[0].length;
			let replacement = handler(match[0]);
			let shadow = ''.padStart(replacement.length, ' ');
			
			str = this.replaceAt(str, startIndex, match[0].length, replacement);
			remainder = this.replaceAt(remainder, startIndex, match[0].length, shadow);
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
			let result = this.applyRule(str, remainder, rule.pattern, rule.handler);
			str = result.result;
			remainder = result.remainder;
		}

        return str;
	}
}

/**
 * Applies general formatting style and colouring
 */
class SampleFormatter extends CodeFormatter {
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
     * Formats a span with a colour
     * @param {String} contents String to format
     * @param {String} colour Colour code
     */
    colouredSpan(contents, colour) {
        return `<span style="color:${colour}">${contents}</span>`;
    }

    /**
     * Format the while sample
     * @param {String} sample Sample contents
     * @returns Formatted sample
     */
    formatWholeSample(sample) {
        return `<pre style="${this.SampleStyle}"><code>${sample}</code></pre>`;
    }
}

/**
 * Formatter for Lavendeux code
 */
export class LavendeuxFormatter extends SampleFormatter {
	constructor() {
		super();

        // General syntax
		this.addRule(/\/\/.*?$/gm, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/\/\*.*?\*\//gms, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/(?:^|[^0-9.,$])0[xXbBoO]?[a-zA-Z0-9]+/gm, (s) => this.colouredSpan(s, this.Colours.radix));
		this.addRule(/@[0-9A-Za-z_]+/gm, (s) => this.colouredSpan(s, this.Colours.decorator));

        // Functions
		this.addRule(/[0-9A-Za-z_]+\(/gm, (s) => `<span style="color:${this.Colours.function}">${s}`);
		this.addRule(/\)/gm, (s) => `${s}</span>`);

        // Values and strings
        this.addRule(/(?:"(?:\\.|[^"']|')*?")|(?:'(?:\\.|[^"']|")*?')/gm, (s) => this.colouredSpan(s, this.Colours.string));
		this.addRule(/[$¥€£0-9A-Za-z._,]+(?:[eE]?[-+]?[0-7])?/gm, (s) => this.colouredSpan(s, this.Colours.data));

        this.addRule(null, (s) => this.formatWholeSample(s)); 
	}
}

/**
 * Formatter for JS code
 */
export class JavascriptFormatter extends SampleFormatter {
	constructor() {
		super();
        // General syntax
		this.addRule(/\/\/.*?$/gm, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/\/\*.*?\*\//gms, (s) => this.colouredSpan(s, this.Colours.comment));
		this.addRule(/(?:^|[^0-9.,$])0[xXbBoO]?[a-zA-Z0-9]+/gm, (s) => this.colouredSpan(s, this.Colours.radix));

        // Functions
		this.addRule(/[0-9A-Za-z_]+\(/gm, (s) => `<span class="function">${s}`);
		this.addRule(/\)/gm, (s) => `${s}</span>`);

        // Strings
		this.addRule(/(?:"(?:\\.|[^"']|')*?")|(?:'(?:\\.|[^"']|")*?')/gm, (s) => this.colouredSpan(s, this.Colours.string));
        
        // Values
		this.addRule(/\W(return|function)\W/gm, (s) => this.colouredSpan(s, this.Colours.decorator));
		this.addRule(/[$¥€£0-9A-Za-z._,]+(?:[eE]?[-+]?[0-7])?/gm, (s) => this.colouredSpan(s, this.Colours.data));

        this.addRule(null, (s) => this.formatWholeSample(s)); 
	}
}