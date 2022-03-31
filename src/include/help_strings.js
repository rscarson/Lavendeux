import { LavendeuxFormatter, JavascriptFormatter } from './formatter';

const lavendeuxFormatter = new LavendeuxFormatter();
const javascriptFormatter = new JavascriptFormatter();

/**
 * Represents a code sample
 */
class Sample {
    constructor(preamble, sample, formatter) {
        this.formatter = formatter;
        this.preamble = preamble;
        this.sample = sample;
    }

    /**
     * Format the code sample preamble
     * @returns The formatted preamble for the code sample
     */
    preambleToString() {
        return this.preamble.map(s => `<p>${s}</p>`).join('\n');
    }

    /**
     * Format the code sample's source code
     * @returns The formatted source code sample
     */
    sampleToString() {
        return this.formatter.format(this.sample);
    }
    
    /**
     * Format the code sample
     * @returns The formatted code sample
     */
    toString() {
        return this.preambleToString() + this.sampleToString();
    }
}

const DataTypesSample = new Sample([
    'A number of datatypes are supported:'
], `
// Integer, floating point or scientific notation numbers
5 + 5.56 + .2e+3

// Currency values
// Note that no exchange rate is being applied automatically
$1,000.00 == ¥1,000.00

// Scientific numbers can be represented a number of ways
5.6e+7 - .6E7 + .2e-3

// Booleans
in_range = 5 > 3 && 5 < 10
true || false

// Integers can also be represented in base 2, 8 or 16
0xFFA & 0b110 & 0777

// Strings are also supported
concat("foo", "bar")
`, lavendeuxFormatter);

const OperationsSample = new Sample([
    'Beyond the simpler operators, the following operations are supported:'
], `
5 ** 2 // Exponentiation
6 % 2 // Modulo
3! // Factorial

// Bitwise operators AND, OR, and XOR:
0xF & 0xA | 0x2 ^ 0xF

// Bitwise SHIFT, and NOT
0xF << 1
0x1 >> 2
~0xA

// Boolean operators
true || false && true
1 < 2 > 5 // true
`, lavendeuxFormatter);

const VariablesSample = new Sample([
    'You can also assign values to variables to be used later: <br />'
    + 'They are case sensitive, and can be composed of underscores or alphanumeric characters'
], `
// You can also assign values to variables to be used later
x = 0xFFA & 0xFF0
x - 55 // The result will be 200

// A few constants are also pre-defined
value = pi * e * tau

// You can also define functions
f(x) = 2*x**2 + 3*x + 5
f(2.3)

// Recursive functions work too!
factorial(x) = x==0 ? 1 : (x * factorial(x - 1) )
factorial(5)
`, lavendeuxFormatter);

const DecoratorsSample = new Sample([
    'Decorators can be put at the end of a line to change the output format. Valid decorators include:'
], `
255 @hex // The result will be 0xFF
8 @oct // The result will be 0o10
5 @float // The result will be 5.0
5 @usd // Also works with @dollars @cad, @aud, @yen, @pounds, or @euros
`, lavendeuxFormatter);

const FunctionsSample = new Sample([
    'The following functions are supported by default:'
], `
// String functions
concat("s1", "s2", ...) | strlen("string") | substr("string", start, [length])
uppercase("s1") | lowercase("S1") | trim("    s1    ")

// Rounding functions
ceil(n) | floor(n) | round(n, precision)

// Trigonometric functions
tan(r), cos(r), sin(r), atan(r), acos(r), asin(r), tanh(r), cosh(r), sinh(r)

// Rounding functions
ln(n) | log10(n) | log(n, base)
sqrt(n) | root(n, base)

// RNG functions
choose("argument 1", 2, 3.0, ...) | rand() | rand(min, max)

// Networking functions
get(url, ["header-name=value", ...]) | post(url, ["header-name=value", ...]) | resolve(hostname)

// Misc. functions
to_radians(degree_value) | to_degrees(radian_value) | abs(n) | tail(filename, [lines]) | time()
`, lavendeuxFormatter);

const ExtensionSample = new Sample([
    'Lavendeux can be extended with javascript. Extensions are run in a sandboxed environment, with no network or host access.<br/>'
    + 'Below is an example of a simple extension:'
], `
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
        
        functions: {
            "callable_name": "js_function_name"
        },
        
        decorator: {
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
`, javascriptFormatter);

export const Samples = [
    DataTypesSample, 
    OperationsSample, VariablesSample, 
    DecoratorsSample, FunctionsSample, 
    ExtensionSample
];