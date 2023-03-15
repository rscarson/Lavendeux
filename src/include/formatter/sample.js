import { LavendeuxFormatter } from './lavendeux';
import { JavascriptFormatter } from './javascript';

const FormatterInstances = {
	lavendeux: new LavendeuxFormatter(),
	javascript: new JavascriptFormatter(),
};
FormatterInstances.get = name => ((name in FormatterInstances)
	? FormatterInstances[name]
	: FormatterInstances.lavendeux);

/**
 * A code sample object
 */
export class JSONSample {
	constructor(sampleJSON) {
		this.name = sampleJSON.name;
		this.format = sampleJSON.format;
		this.shortDescription = sampleJSON.short_desc;
		this.description = sampleJSON.description.join('\n');
		this.code = sampleJSON.code.join('\n');

		this.formatter = FormatterInstances.get(this.format);
		this.html = this.toHTML();
	}

	/**
     * Retrieve all code samples from a json object
     * @param {Object} sourceObject
     * @returns Array of code sample objects
     */
	static allFromJSON(sourceObject) {
		return sourceObject.samples.map(e => new JSONSample(e));
	}

	/**
	 * Formats the description
	 * @returns HTML formatted description
	 */
	descriptionToHTML() {
		return this.formatter.tagWrap(this.description.replaceAll('\n', '<br/>'), 'p');
	}

	/**
	 * Formats the code sample
	 * @returns HTML formatted code sample
	 */
	codeToHTML() {
		return this.formatter.format(this.code);
	}

	/**
     * Formats the entire sample
     * @returns HTML string
     */
	toHTML() {
		return `${this.descriptionToHTML()}\n${this.codeToHTML()}`;
	}
}
