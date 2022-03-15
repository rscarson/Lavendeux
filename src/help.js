import React from 'react'
import './App.css'

import { Container } from 'react-bootstrap';

const dataTypeSample = `
// Integer, floating point or scientific notation numbers
5 + 5.56 + .2e+3

// Scientific numbers can be represented a number of ways
5.6e+7 - .6E7 + .2e-3

// Integers can also be represented in base 2, 8 or 16
0xFFA & 0b110 & 0777

// Strings are also supported
concat("foo", "bar")
`;

const operationsSample = `
5 ** 2 // Exponentiation
6 % 2 // Modulo
3! // Factorial

// Bitwise operators AND, OR, and XOR:
0xF & 0xA | 0x2 ^ 0xF

// Bitwise SHIFT, and NOT
0xF < 1
0x1 > 2
~0xA
`;

const variablesSample = `
// You can also assign values to variables to be used later
x = 0xFFA & 0xFF0
x - 55 // The result will be 200

// A few constants are also pre-defined
value = pi * e * tau
`;

const decoratorsSample = `
255 @hex // The result will be 0xFF
8 @oct // The result will be 0o10
5 @float // The result will be 5.0
`;

const functionsSample = `
// String functions
concat("s1", "s2", ...) | strlen("string") | substr("string", start, [length])

// Rounding functions
ceil(n) | floor(n) | round(n, precision)

// Trigonometric functions
tan(r), cos(r), sin(r), atan(r), acos(r), asin(r), tanh(r), cosh(r), sinh(r)

// Rounding functions
ln(n) | log10(n) | log(n, base)
sqrt(n) | root(n, base)

// Misc. functions
to_radians(degree_value) | abs(n)
`;

const jsSample = `
/**
* This function tells Lavendeux about this extension.
* It must return an object similar to the one below.
* @returns Object
*/
function extension() }
    return {
        name: "Extension Name",
        author: "Author's name",
        version: "0.0.0",
        
        functions: {,
            "callable_name": "js_function_name"
        },
        
        decorator: {,
            "callable_name": "js_decorator_name"
        },
    }
}

/**
* This function can be called from Lavendeux as callable_name(...)
* args is an array of value objects with either the key Integer, Float or String
* It must also return an object of that kind, or throw an exception
* @returns Object
*/
function js_function_name(args) }
    return {
        "Integer": 5,
    };
}

/**
* This decorator can be called from Lavendeux as @callable_name
* arg is a value object with either the key Integer, Float or String
* It must return a string, or throw an exception
* @returns String
*/
function js_decorator_name(arg) {
    return "formatted value";
}
`;

function Help(props) {
	function replaceAt(str, search, index, replacement) {
		return str.substring(0, index) + str.substring(index).replace(search, replacement);
	}

	function encode(str) {
		//str = str.split('\n').map(s => s.trim()).join('\n');
		let result = str;

		let match; let regexp;

		regexp = new RegExp('\\/\\/.*','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="comment">${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('\\/\\*.*?\\*\\/','gms');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="comment">${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('0[xXbBoO]?[a-zA-Z0-9]+','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="radix">${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('\\@[0-9A-Za-z_]+','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="decorator">${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('[0-9A-Za-z_]*\\(','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="function">${match[0]}`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('\\)','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		regexp = new RegExp('\\"?\\\'?[0-9A-Za-z\\._]+[eE]?[\\-\\+]?[0-7]*\\"?\\\'?','gm');
		while ((match = regexp.exec(str)) !== null) {
			let replacement = `<span class="data">${match[0]}</span>`;
			str = replaceAt(str, match[0], match.index, ''.padStart(replacement.length, ' '));
			result = replaceAt(result, match[0], match.index, replacement);
		}

		return `<pre class="code-sample">${result}</code></pre>`;
	}

	function renderSample(sample) {
		return (
			<p dangerouslySetInnerHTML={{__html: encode(sample)}}></p>
		);
	}
	
    return (
		<Container>
			<p>A number of datatypes are supported:</p>
			{renderSample(dataTypeSample)}

			<p>Beyond the simpler operators, the following operations are supported:</p>
			{renderSample(operationsSample)}

			<p>
				You can also assign values to variables to be used later:
				<br />They are case sensitive, and can be composed of underscores or alphanumeric characters
			</p>
			{renderSample(variablesSample)}

			<p>
				Decorators can be put at the end of a line to change the output format. Valid decorators include:
				<br /><strong>@bin, @oct, @hex, @int, @float, or @sci</strong>
			</p>
			{renderSample(decoratorsSample)}

			<p>
				The following functions are supported by default:
			</p>
			{renderSample(functionsSample)}

			<p>
				Lavendeux can be extended with javascript. Extensions are run in a sandboxed environment, with no network or host access.<br/>
				Below is an example of a simple extension:
			</p>
			{renderSample(jsSample)}
		</Container>
	);
}

export default Help;