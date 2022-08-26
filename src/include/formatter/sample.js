/**
 * A code sample object
 */
export class JSONSample {
	constructor(name, format, description, code) {
		this.name = name;
		this.format = format;
		this.description = description;
		this.code = code;
	}

	/**
     * Retrieve a code sample from a json object
     * @param {Object} sampleObject
     * @returns Code sample object
     */
	static fromJSON(sampleObject) {
		return new JSONSample(
			sampleObject.name,
			sampleObject.format,
			sampleObject.description.join('\n'),
			sampleObject.code.join('\n'),
		);
	}

	/**
     * Retrieve all code samples from a json object
     * @param {Object} sourceObject
     * @returns Array of code sample objects
     */
	static allFromJSON(sourceObject) {
		return sourceObject.samples.map(e => this.fromJSON(e));
	}

	/**
	 * Formats the description
	 * @returns HTML formatted description
	 */
	descriptionToHTML(formatter) {
		return formatter.tagWrap(this.description.replaceAll('\n', '<br/>'), 'p');
	}

	/**
	 * Formats the code sample
	 * @returns HTML formatted code sample
	 */
	codeToHTML(formatter) {
		return formatter.format(this.code);
	}

	/**
     * Formats the entire sample
     * @returns HTML string
     */
	toHTML(formatter) {
		return `${this.descriptionToHTML(formatter)}\n${this.codeToHTML(formatter)}`;
	}
}
