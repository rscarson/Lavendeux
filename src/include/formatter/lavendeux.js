import { Formatter } from './formatter';

/**
 * Formatter for Lavendeux code
 */
export class LavendeuxFormatter extends Formatter {
	constructor() {
		super();

		// Comments
		this.addRule(/\/\/.*?$/, s => this.span(s, 'comment'));
		this.addRule(/\/\*.*?\*\//s, s => this.span(s, 'comment'));

		// Functions and decorators
		this.addRule(/@[0-9A-Za-z_]+/, s => this.span(s, 'decorator'));
		this.addRule(/[0-9A-Za-z_]+\(/, s => this.span(s, 'function', true));
		this.addRule(/\)/, s => `${s}</span>`);

		// Values and strings
		this.addRule(/(?:"(?:\\.|[^"']|')*?")|(?:'(?:\\.|[^"']|")*?')/, s => this.span(s, 'string'));
		this.addRule(/(?:[0-9.,$])0[xXbBoO]?[a-zA-Z0-9]+/, s => this.span(s, 'radix'));
		this.addRule(/[$¥€£0-9A-Za-z._,]+(?:[eE]?[-+]?[0-7])?/, s => this.span(s, 'data'));

		// Wrap sample
		this.addRule(null, s => this.outer(s));
	}
}
