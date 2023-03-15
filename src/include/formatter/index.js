import sampleJSON from '../../data/samples.json';

import { LavendeuxFormatter } from './lavendeux';
import { JavascriptFormatter } from './javascript';
import { JSONSample } from './sample';

export { Formatter } from './formatter';
export { JSONSample } from './sample';
export { LavendeuxFormatter } from './lavendeux';
export { JavascriptFormatter } from './javascript';

export const FormatterInstances = {
	lavendeux: new LavendeuxFormatter(),
	javascript: new JavascriptFormatter(),
};
FormatterInstances.get = name => ((name in FormatterInstances)
	? FormatterInstances[name]
	: FormatterInstances.lavendeux);

/**
 * Get all sample data
 * @returns All JSON formatted samples
 */
export function getAllSamples() {
	return JSONSample.allFromJSON(sampleJSON);
}

/**
 * Format all sample data
 * @returns All HTML formatted samples
 */
export function getAllFormattedSamples() {
	return getAllSamples().map(s => s.toHTML(
		FormatterInstances.get(s.formatter),
	));
}

/**
 * Format all sample data
 * @returns All HTML formatted samples
 */
export function getSampleHTML() {
	return getAllFormattedSamples().join('\n');
}

/**
 * Get code example
 * @returns Code example
 */
export function getExampleSample() {
	return sampleJSON.example.join('\n');
}
