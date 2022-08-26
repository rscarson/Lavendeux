/**
 * Generic code formatter
 * Applies a set of formatting rules to an input string
 */
export class Formatter {
	constructor() {
		this.rules = [];
	}

	/**
     * Wraps a sample in an HTML tag
     * @param {String} inner
     * @param {String} tagName
     * @param {String} className
     * @param {Boolean} skipClosing
     * @returns Wrapped sample
     */
	tagWrap(inner, tagName, className = '', skipClosing = false) {
		return `<${tagName} class='${className}'>${inner}${skipClosing ? '' : `</${tagName}>`}`;
	}

	/**
     * Wraps a sample in a coloured span
     * @param {String} inner
     * @param {String} className
     * @param {Boolean} skipClosing
     * @returns Wrapped sample
     */
	span(inner, className, skipClosing = false) {
		return this.tagWrap(inner, 'span', className, skipClosing);
	}

	/**
     * Wrap the whole sample
     * @param {String} inner
     * @returns Wrapped sample
     */
	outer(inner) {
		return this.tagWrap(
			this.tagWrap(inner, 'code'),
			'pre',
			'formatted-sample',
		);
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
			pattern,
			handler: replacement,
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
                + (rule.pattern.global ? '' : 'g')
                + (rule.pattern.multiline ? '' : 'm'));
		} else {
			// Whole string rule
			return {
				result: rule.handler(target),
				remainder: rule.handler(target),
			};
		}

		// Regex rule
		while ((match = regexp.exec(remainder)) !== null) {
			const startIndex = regexp.lastIndex - match[0].length;
			const replacement = rule.handler(match[0]);
			const shadow = ''.padStart(replacement.length, ' ');

			str = this.replaceAt(str, startIndex, match[0].length, replacement);
			remainder = this.replaceAt(remainder, startIndex, match[0].length, shadow);
			regexp.lastIndex = startIndex + replacement.length;
		}

		return {
			result: str,
			remainder,
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
			const result = this.applyRule(str, remainder, rule);
			str = result.result;
			remainder = result.remainder;
		}

		return str;
	}
}
