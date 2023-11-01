## Basic Syntax
Lavendeux is mostly geared towards small mathematical expressions. It supports function calls, output formating, various bases, as well as many types of data.

The sample below demonstrates a few of these features, to set up some variables, and define a function:
```javascript
    FLAG_FEATURE1 = 0b10000000
    FLAG_FEATURE2 = 0b01000000
    FLAG_FEATURE3 = 0b00100000
    has_flag(input, flag) = (input & flag) == flag
```

Now we can use these to display our values as hexadecimal, and to check if our value contains a flag:
```javascript
    bits = FLAG_FEATURE1 | FLAG_FEATURE2 @hex
    has_flag(bits, FLAG_FEATURE1)
    has_flag(bits, FLAG_FEATURE3)
```

---

There are a lot of functions and operators built in - for a complete list please see the 'operators' and 'functions' sections.

Here are a few examples:
```javascript
    sqrt(16) ** 2 == 16     // Exponentiation
    2 * 3 == 6              // Multiplication
    2 << 1 == 4             // Bitwise shift
    0x2 ^ 0x3 == 0x1        // Bitwise XOR
    [1, 2] + 2 == [3, 4]    // Array OPs
```

Data is also accepted in a variety of formats, including but not limited to:
```javascript
    $1,000 + ¥1,000.00 == 2,000     // Currencies
    5.6e+7 - .6E7 + .2e-3           // Sci notation
    1 + 1.2 + .3 == 2.5             // Floats and integers
    5 > 3 == true                   // Boolean
    concat('foo', 'bar')            // String
    [1, 2, 'test']                  // Arrays
    {'key': 1, 2: 'value'}          // Objects
```

## Features
Functions can be used, or defined to simplify expressions. Below is a sample showing a few built-in functions, and how to define your own:
```javascript
    f(x) = 3x**2 + 2x + 1; f(3) == 34
    format_name(first, last) = concat(first, " ", last)
    x = [1, 2, 3, 4, 5]; pop(x) == 5; len(x) == 4
```

---

As seen above, variables can be defined to store values for later. Lavendeux uses weak-typing, so most operators will work on most combination of types:
```javascript
    float_v = 0.02
    int_v = 7
    string_v = "test"

    float_v + int_v == 7.02
    float_v + string_v == "0.02test"
```

---

The format of the resulting string can be altered by adding an @decorator to the end of the expression - here are a few examples:
```javascript
    /* 0xFF */ 15 @hex
    /* $20,000.00 */ 20000 @usd
    /* 0b11 */ 3 @bin
```

---

Compound types, such as arrays and objects are also supported, and can be formed from any combination of types:
```javascript
    arr = [1, 2.0, 'test', false]
    obj = {0: 'test', 'test': 1, false:true}
```

## Datatypes

Lavendeux provides the following types: integer, float, string, boolean, arrays, and objects. Any type can be cooerced into any other type - EXCEPT for numbers, which cannot be built from non-numeric types

Lavendeux is loosely typed - which means that most operations will accept most combinations of types. Below are some examples of type cooersion Lavendeux provides:
```javascript
    1 != 1.2 && 1 == 1.0
    "1" == 1
    0 == false
    [1] == 1
    [1] == {0: 1}
```

The example below shows the various representations of the available types:
```javascript
    /* Integers */  1000000; 1,000,000
    /* Floats */    0.2; .2, 2.5e-1; 2.5e+1; 2e4; $1; $1.20
    /* Booleans */  false; true
    /* Strings */   'test'; 'test\''; "test"
    /* Arrays */    []; [1, 'test']
    /* Objects */   {}; {0: 1}; {'test': false}
```

---

## Operators
Lavendeux supports a variety of operators to suit a wide set of usecases

---

Bitwise operators
```javascript
    /* AND */   0x0F & 0x0A == 0x0A
    /* OR */    0x0F | 0xAF == 0xAF
    /* XOR */   0x0F ^ 0x0A == 0x05
    /* SHIFT */ 0x0F << 1 == 0x1E; 0x0F >> 1 == 0x07
    /* NOT */   ~0x0A == 0x05
```

---

Boolean logic operators
```javascript
    /* AND */  true && true == true
    /* OR */   true || false == true
    /* EQ */   5 == 5; 3 != 5
    /* GT */   3 < 5; 3 <= 3
    /* LT */   5 > 3; 3 >= 3
```

---

Mathematical operators
```javascript
    /* ADD */        1 + 2 + 3 == 6
    /* SUB */        3 - 2 - 1 == 0; -2; -(3!)
    /* MUL */        5 * 3 == 15; x=2; 2x == 4; (2)(3) == 6
    /* DIV */        3 / 2.0 == 1.5
    /* POW */        2**4 == 16
    /* MODULO */     3 % 2 == 1
    /* FACTORIAL */  4! == 24
```

---

Other operators, including ternary expressions, assignment expressions and array indexing:
```javascript
    x = 5
    f(x) = 5x
    x=[1,2]; x[0] == 1
    y={'test': 4}; y['test'] == 4
    
    len(x) ? true : false
    true ? 1 : 2
```

## Functions

There are 3 types of functions in Lavendeux - built-in functions provided by default, extension-provided functions, and user-defined functions
```javascript
    f(x) = 2x**2 + 3x + 4       // User defined function
    f(3) == 31

    strlen("test") == 4         // Built-in function

    "start" @zarban             // Example function provided by an extension
```

Below is a list of provided built-in functions:
```javascript
    Math Functions
    ==============
    abs(n): Returns the absolute value of n
    acos(n): Calculate the arccosine of n
    array(n): Returns a value as an array
    asin(n): Calculate the arcsine of n
    atan(n): Calculate the arctangent of n
    bool(n): Returns a value as a boolean
    ceil(n): Returns the nearest whole integer larger than n
    cos(n): Calculate the cosine of n
    cosh(n): Calculate the hyperbolic cosine of n
    float(n): Returns a value as a float
    floor(n): Returns the nearest whole integer smaller than n
    int(n): Returns a value as an integer
    ln(n): Returns the natural log of n
    log(n, base): Returns the logarithm of n in any base
    log10(n): Returns the base 10 log of n
    max(n1, n2): Returns the largest numeric value from the supplied arguments
    min(n1, n2): Returns the smallest numeric value from the supplied arguments
    root(n, base): Returns a root of n of any base
    round(n, [precision]): Returns n, rounded to [precision] decimal places
    sin(n): Calculate the sine of n
    sinh(n): Calculate the hyperbolic sine of n
    sqrt(n): Returns the square root of n
    tan(n): Calculate the tangent of n
    tanh(n): Calculate the hyperbolic tangent of n
    to_degrees(n): Convert the given radian value into degrees
    to_radians(n): Convert the given degree value into radians

    Misc Functions
    ==============
    atob(input): Convert a string into a base64 encoded string
    btoa(input): Convert a base64 encoded string to an ascii encoded string
    call(filename): Run the contents of a file as a script
    help([function_name]): Display a help message
    prettyjson(input): Beautify a JSON input string
    run(expression): Run a string as an expression
    tail(filename, [lines]): Returns the last [lines] lines from a given file
    time(): Returns a unix timestamp for the current system time
    urldecode(input): Decode urlencoded character escape sequences in a string
    urlencode(input): Escape characters in a string for use in a URL

    Network Functions
    =================
    api(name, [endpoint]): Make a call to a registered API
    api_delete(name): Remove a registered API from the list
    api_list(): List all registered APIs
    api_register(name, base_url, [api_key]): Register a new API for quick usage
    get(url, [headers]): Return the resulting text-format body of an HTTP GET call
    post(url, body, [headers]): Return the resulting text-format body of an HTTP POST call
    resolve(hostname): Returns the IP address associated to a given hostname

    Strings Functions
    =================
    concat([s1, s2]): Concatenate a set of strings
    contains(source, s): Returns true if array or string [source] contains [s]
    lowercase(s): Converts the string s to lowercase
    regex(pattern, subject, [group]): Returns a regular expression match from [subject], or false
    strlen(s): Returns the length of the string s
    substr(s, start, [length]): Returns a substring from s, beginning at [start], and going to the end, or for [length] characters
    trim(s): Trim whitespace from a string
    uppercase(s): Converts the string s to uppercase

    Cryptography Functions
    ======================
    choose(option1, option2): Returns any one of the provided arguments at random
    md5(input1, input2): Returns the MD5 hash of a given string
    rand([m], [n]): With no arguments, return a float from 0 to 1. Otherwise return an integer from 0 to m, or m to n
    sha256(input1, input2): Returns the SHA256 hash of a given string

    Arrays Functions
    ================
    dequeue(array): Remove the first element from an array
    element(input, index): Return an element from a location in an array or object
    enqueue(array, element): Add an element to the end of an array
    is_empty(input): Returns true if the given array or object is empty
    keys(input): Get a list of keys in the object or array
    len(input): Returns the length of the given array or object
    merge(target, inputs1, inputs2): Merge all given arrays or objects
    pop(array): Remove the last element from an array
    push(array, element): Add an element to the end of an array
    remove(input, index): Removes an element from an array
    values(input): Get a list of values in the object or array
```

## Decorators

Since the output of a Lavendeux expression is a string, the format of that string can be altered using @decorators.

Here is a list of the @decorators provided by default:
```javascript
    @array: Format a number as an array
    @bin: Base 2 number formatting, such as 0b11
    @bool/@boolean: Format a number as a boolean
    @bool/@boolean: Format a number as a boolean
    @default: Default formatter, type dependent
    @dollar/@dollars/@usd/@aud/@cad: Format a number as a dollar amount
    @dollar/@dollars/@usd/@aud/@cad: Format a number as a dollar amount
    @dollar/@dollars/@usd/@aud/@cad: Format a number as a dollar amount
    @dollar/@dollars/@usd/@aud/@cad: Format a number as a dollar amount
    @dollar/@dollars/@usd/@aud/@cad: Format a number as a dollar amount
    @euro/@euros: Format a number as a euro amount
    @euro/@euros: Format a number as a euro amount
    @float: Format a number as floating point
    @hex: Base 16 number formatting, such as 0xFF
    @int/@integer: Format a number as an integer
    @int/@integer: Format a number as an integer
    @object: Format a number as an object
    @oct: Base 8 number formatting, such as 0b77
    @ordinal: Format an integer as an ordinal (1st, 38th, etc)
    @percentage/@percent: Format a floating point number as a percentage
    @percentage/@percent: Format a floating point number as a percentage
    @pound/@pounds: Format a number as a pound amount
    @pound/@pounds: Format a number as a pound amount
    @roman: Format an integer as a roman numeral
    @sci: Scientific number formatting, such as 1.2Ee-3
    @utc: Interprets an integer as a timestamp, and formats it in UTC standard
    @yen: Format a number as a yen amount
```

## Extensions

Lavendeux can be extended through javascript plugins that add functions and decorators. A curated list of extensions is available [here](https://rscarson.github.io/lavendeux/extensions).

Below is a simple example of a possible extension that adds one function - add(left, right), and @colour:
```javascript
    let extension = lavendeux.extend({
        'name': 'simple_extension',
        'author': '@rscarson',
        'version': '1.0.0'
    });

    extension.addIntegerFunction(
        'add', 
        (left, right) => left + right
    ).requireArguments(
        lavendeux.Types.Numeric, 
        lavendeux.Types.Numeric
    );

    extension.addIntegerDecorator(
        'colour', 
        (input) => `#${(input & 0x00FFFFFF).toString(16).padEnd(6, '0')}`
    );

    lavendeux.register(extension);
```