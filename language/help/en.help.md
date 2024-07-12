## Getting Started
### Lavendeux enhances the clipboard
**Lavendeux inlines calculating, programming and testing utilities into your favourite text editor**

... It's javascript. There's a javascript engine in your clipboard now.
Now you don't need to switch tabs to do math, test an API endpoint or do unit conversions.

Commands are run as a single block of non-emca javascript, and the result of the last line is returned as the output.
Blocks of input can optionally end with an `@decorator` which will call a function to transform the output.

`help()` will display the set of functions provided by default.
The worspace is persistent, and so functions and values declared will remain available.

## Extensions

Functionality can be added using `extensions`; Small javascript modules that add functions callable from Lavendeux.
Below is an example of a simple extension:
```javascript
/// name = Simple extension
/// description = A simple extension that adds a single function
/// author = Your Name
/// version = 1.0
/// license = MIT

/* ^ All fields above are optional! ^ */

lavendeux.register((s) => `Hello ${s}`, 'sample')
    .with_category("Example")                               // Optional category; default is 'Misc'
    .with_short("A short single line description")          // Optional short description
    .with_description("A longer multiline description")     // Optional longer desc.
    .with_examples(`A multiline set of usage examples`);    // Optional examples
```

Once imported in the `extensions` tab, these functions will be displayed in the entry for the script
They will also be visible in the `help()` function's return value

*Note: These instructions are currently only available in english; if you wish to provide a translation into a different language feel free to provide a pull-request to `https://github.com/rscarson/lavendeux`*

## Functions

The output of the `help()` function is displayed below. It shows all the functions available by default.
### API
- add_api() -> Registers an API. Accepts a string, or an object with the properties [ url, headers, description, examples, auth_key]
- api() -> Returns an API object
- api_key() -> Adds an API key for an API
- del_api() -> Unregisters an API
- list_api() -> Returns a list of registered APIs

### Crypto
- choose(array) -> Returns a random element from a given array
- rand() -> With no arguments, return a float from 0 to 1. Otherwise return an integer from 0 to m, or m to n

### Decorators
- aud(value) -> Interprets a number as a AUD amount
- bin(value) -> Base 2 number formatting, such as 0b101
- bool(value) -> Format a value as a boolean
- cad(value) -> Interprets a number as a CAD amount
- cny(value) -> Interprets a number as a CNY amount
- eur(value) -> Interprets a number as a Euro amount
- gbp(value) -> null
- hex(value) -> Base 16 number formatting, such as 0xFF
- inr(value) -> Interprets a number as a INR amount
- int(value) -> Format a number as an integer
- jpy(value) -> Interprets a number as a JPY amount
- oct(value) -> Base 8 number formatting, such as 0o77
- ordinal() -> Displays an integer as an ordinal number
- percent(value) -> Displays a number as a percentage
- rub(value) -> Interprets a number as a RUB amount
- sci(value) -> Scientific notation formatting, such as 1.2e3
- usd(value) -> Interprets a number as a USD amount
- utc(value) -> Interprets an integer as a timestamp, and formats it in UTC standard

### Math
- abs(n) -> Returns the absolute value of a number
- ceil(n) -> Rounds a number up to the nearest integer
- floor(n) -> Rounds a number down to the nearest integer
- ln(n) -> Returns the natural logarithm of a number
- log(n, base) -> Returns the logarithm of a number with a given base
- log10(n) -> Returns the base 10 logarithm of a number
- max(...args) -> Returns the largest value in a set
- min(...args) -> Returns the smallest value in a set
- root(n, root) -> Returns the nth root of a number
- round(n, precision) -> Rounds a number to a given precision
- sqrt(n) -> Returns the square root of a number

### System
- help() -> Show available functions and their descriptions
- set_number_decimal(s) -> Set the decimal seperator for number formatting
- set_number_seperator(s) -> Set the thousands seperator for number formatting

### Time
- time() -> Returns a unix timestamp for the current system time

### Trig
- acos(n) -> Calculate the acos of n, in radians
- asin(n) -> Calculate the asin of n, in radians
- atan(n) -> Calculate the atan of n, in radians
- atan2(y, x) -> Arctangent2 of a number pair, in radians
- cos(n) -> Calculate the cos of n, in radians
- cosh(n) -> Calculate the hyperbolic cos of n, in radians
- sin(n) -> Calculate the sin of n, in radians
- sinh(n) -> Calculate the hyperbolic sin of n, in radians
- tan(n) -> Calculate the tan of n, in radians
- tanh(n) -> Calculate the hyperbolic tan of n, in radians
- to_degrees(n) -> Converts radians to degrees
- to_radians(n) -> Converts degrees to radians

### UI
- titled_box() -> Draw a titled box with unicode line chars

