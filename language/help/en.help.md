# Lavendeux - Language Documentation

This document will provide information on lavendish, a language focused on short, single-line expressions designed to manipulate values.
It was created for use in [Lavendeux](https://rscarson.github.io/lavendeux/).

Inputs are a series of expressions separated by a newline, or a `;`.
Lines can optionally end with an @decorator to format the output as a string (see `section 3.2`)

Comments are either `//`, which turns the rest of the line into a comment
Or a block comment bounded by /* and */

## Chapter 1 - Values

### 1.1 Data Types

All expressions in Lavendeux will return a value of a specific type. These types can be broadly categorized as `numeric`, `compound`, or `string`.

#### Numeric Types

The first group of types are classified as numeric; they can all freely be cast to each other.  
If a type is downcast to a smaller numeric type, it will be truncated to fit (1.6 becomes 1 for example)

But expressions will always upgrade both values to the highest-order in this list (currency being the highest, bool, the lowest):
- Bool: a single-bit truth value [`true`, `false`]
- Int: A 64bit integer [`1`, `2,000`, `-78`]
- Float: A 64bit floating-point number [`1.0`, `.2`, `3e+7`]
- Fixed: A fixed-point decimal value [`1.22D`, `4D`]
- Currency: A fixed-point decimal value with a currency symbol [`$1`, `$2.00`, `3￥`]

**Bool is an outlier here, since any type can be cast to bool**: Truth is determined by equivalence to 0, or by emptiness, depending on the type.

Examples:
```lavendeux
    $1,000 + ¥1,000.00 == 2,000     // Currencies
    5.6e+7 - .6E7 + .2e-3           // Sci notation
    1 + 1.2 + .3 == 2.5             // Floats and integers
    5 > 3 == true                   // Boolean
    'foo' + 'bar'                   // String
    [1, 2, 'test']                  // Arrays
    {'key': 1, 2: 'value'}          // Objects
```

#### Compound Types

The second group are compound types, which encapsulate a set of values

Compound types include:
- Array: An ordered set of values
- Object: A set of values indexed by a non-compound value - **it is an error to use a compound type as an object key**
- Range: A special value which cannot be indexed into directly, and will always evaluate as an array in comparisions and operations. All ranges are inclusive;

Attempting to convert non-compound types into one of these will result in a single-value array or object.

For example, `1 as array` would result in `[1]`,  
and `1 as object` would be the equivalent to `[1] as object`, which is `{0: 1}`  
**Note**: Non-compound types are first cast to array before being transformed into objects.

Attemting to convert a compound value into a non-compound type will only work if the length of the compound value is 1, and will simply extract that value:
```lavendeux
1 as array == [1]
[1] as int == 1

1 as object == {0: 1}
{0: 1} as int == 1
```

#### String Values

The last value is string, which any value can be cast to in order to get a string representation.

You can use the `is` keyword to check the type of a value:
`1 is int`, for example, or `'test' is string`

Examples:
```lavendeux
2,000 as string == '2000'
$1.00 as string == '$1.00'
2.0 as string == '2.0'
[1, 2, 3] as string == '[1, 2, 3]'
{'test': 2} as string == '{test: 2}'
(0..10) as string == '0..10'
```

### 1.2 Formats and Examples

Here are the input formats supported when using the above types:

**Integers**
-------
- Base-10, such a `10`, with optional commas for thousands-seperators: `10,000`  
- Other bases, such as binary (`0b101010101`), hex (`0xFFA`), or octal (`0777` or `0o6`)  

**Floats**
-------
- Regular notation, leading number is optional: `5.22` or `.3`  
- Sci notation: `5e+1`, `5E-2`, `6.2e3`  

**Decimal**
-------
- Fixed-point literal: `1D`, `2.3323d`  
- Currency value: `$1.00`, `3￥`  

Supported Currency symbols:
```
$ | ¢ | £ | ¤ | ¥ | ֏ | ؋ | ߾ | ߿ | ৲ | ৳ | ৻ | ૱ | ௹ | ฿ | ៛ | ₠ | ₡ |
₢ | ₣ | ₤ | ₥ | ₦ | ₧ | ₨ | ₩ | ₪ | ₫ | € | ₭ | ₮ | ₯ | ₰ | ₱ | ₲ | ₳ |
₴ | ₵ | ₶ | ₷ | ₸ | ₹ | ₺ | ₻ | ₼ | ₽ | ₾ | ₿ | ꠸ | ﷼ | ﹩ | ＄ | ￠ |
￡ | ￥ | ￦
```

**Bool**
-----
`true` or `false`, case-insensitive

**String**
-----
Single or double quote enclosed; `'test'` or `"test"`  
With the following supported escape sequences:
- `\'` Single-quote
- `\"` Double-quote
- `\n` Newline
- `\r` Carriage-return
- `\t` Tab
- `\\` Literal backslash

**Array**
-----
Square bracket enclosed, comma separated; `[2, 3]`

**Object**
-----
Curly-brace enclosed comma separated pairs of `k:v`  
Where key can be any type except array, object or range  
`{0: 1, true: 'test', 1.2: 'no'}`  

**Range**
-----
Pair of integers or characters split by `..`  
`0..10`; 0 to 10 inclusive  
`'a'..'c'`; The array `['a', 'b', 'c']`  

### 1.3 Converting between types

You can manually convert between types using `<value> as <type>`, so long as that conversion is allowed:
- Numeric values can always convert to other numeric values [`1 as float`, `true as fixed` or `$1.00 as int` are all valid]
- Non-compound non-numeric values cannot convert into numeric values [`'test' as int` would be an error]
- Any type `T` can be converted to an array as `[T]`, or an object as `{0: T}`
- Conversely, single-element compound values such as `[T]` or `{0: T}` can be freely converted to any type valid for `T`
- All types can be converted to string or bool
- Range can become string, bool or array, but no type can become range

Comparisons and expressions will always try and cooerce both values to the same type, using these rules, in this order:
- If either value is a range, compare the values as arrays
- If either value is an object, compare the values as objects
- If either value is an array, compare the values as arrays
- If either value is an string, compare the values as strings
- If either value is an currency, compare the values as currencies
- If either value is a fixed-point, compare the values as fixed-points
- If either value is an float, compare the values as floats
- If either value is an int, compare the values as ints
- If either value is a bool, compare the values as bools

## Chapter 2 - Operations

Operations in lavendish can be broadly split into a few categories

### 2.1 Arithmetic operations

All the normal arithmetic operations (+, -, *, /, and %), as well as exponentiation (**) are supported by all the numeric types.
The resulting type of an expression will be determined using the system described in `section 1.2`

Lavendish also supports implicit muliplication; so the expressions `2 * x` and `2x` are equivalent to each other.

You can also use addition for string, array, or object concatenation:
```lavendeux
"hello" + " " + "world"
[1,2] + [3]
```

Subtraction for search and deletion:
```lavendeux
"hello world" - "world"
[0, 1, 2] - [1,2]
{'a': 1, 'b': 1, 'c': 2} - 1
```

Negation for string, or array inversion:
```lavendeux
-"hi" == "ih"
-[1,2] == [2,1]
```

### 2.2 Bitwise operations

All bitwise operations are performed on, and result in integers. If an operand cannot be cast to integer it will result in an error.
Booleans will be upgraded to int, and all other types will be truncated to int
Available operators:
- `~` bitwise not
- `^` bitwise xor
- `|` bitwise or
- `&` bitwise and
- `>>` right-shift
- `<<` left-shift

Here is an example that uses these concepts for bit-level flags:
```lavendeux    
FLAG_FEATURE1 = 0b10000000
FLAG_FEATURE2 = 0b01000000
FLAG_FEATURE3 = 0b00100000
bits = FLAG_FEATURE1 | FLAG_FEATURE2
```

`bits` will now contain the value 192 (0xC0)

### 2.3 Boolean operations

All boolean operations are performed on, and result in booleans. All other types will be downgraded to boolean (by casting)
Available operators:
- `!` NOT
- `&&` AND
- `||` OR
- `==, !=, >= >, <=, <` Comparison

Examples - all of these evaluate to true
```lavendeux
[] == false
[0] as bool == true
65 > 6.5
```

Short-circuit evaluation is used by these operators, to prevent side-effects:
```lavendeux
x = 3
true || assign('x', 5)
x == 3

false && assign('x', 5)
x == 3
```

### 2.4 Indexing

You can index into array, objects, or strings:
```lavendeux
arr = [1, 2, 3]
obj = {'a': 1, 'b': 2, 'c': 3}
str = "hello world"
```

You can also set indices, or get ranges or arrays of indices
```lavendeux
arr[1] = 4; // Setting an index
obj['a'..'b'] // Grabbing a set of indices
str[ arr ] // Does not need to be a range!

del arr[0] // The delete keyword also works on indices for array and object
```

### 2.5 Matching

Matching expressions of the form `value <match operator> pattern` can also be used to search for a subvalue inside of an array, object or string.

**contains**
-----
True if the pattern can be found anywhere in the value
```lavendeux
'test' contains /st/
[1,2] contains 1
{0: 1} contains 1
```

**starts_with**
-----
True if the pattern can be found at the beginning of the value
```lavendeux
'test' starts_with 't'
[1,2] startswith 1
```

**ends_with**
-----
True if the pattern can be found at the beginning of the value
```lavendeux
'test' ends_with 't'
[1,2] endswith 2
```

**matches**
-----
True if the pattern matches the whole value
```lavendeux
'test' matches /T../i
[1,2] matches [1,2]
{0: 1, 1: 2} matches [1,2]
```

## Chapter 3 - Variables, Functions and Loops

### 3.1 Variables

As you've seen above, values can be stored and retrieved. Variable named must start with a letter or underscore and can also contain numbers.

In addition to the normal `variable = value` syntax, you can store variables in these ways:
- Destructuring: `(a, b, c) = array` 
- Indexed: `arr[index] = value`
- Functional: `name(a, b, c) = a + b + c` (more on this later)

Variables can also be removed using the keyword `del` or `unset`

Examples:
```lavendeux
array_value = [1, 2, 3]
array_value[3] = 4
(a, b, c, d) = array_value
```

### 3.2 Decorators

Decorators are placed at the end of a line, and will format that line's output as a string. They are provided by the standard library, or by extensions.
Here are some examples:

```lavendeux
2 @usd          // $2.00
56 @ordinal     // 56th
0.432 @percent  // 43.2%
12 @roman       // XII
time() @utc     // 2024-01-11 14:07:08 
```

### 3.3 Functions

Functions in lavendeux come in 3 flavours: stdlib, extension, and user-defined  
They can be called with `function_name(arg1, arg2)`  
A few examples:
```
atob('test')        // Encode a string as base64 
time()              // Get the current unix timestamp
choose(1..10000)    // Select an item as random from the input
```

User-defined functions can be created with `name(a, b, c) = a + b + c`

Functions can be recursive:
```lavendeux
factorial(x) = x==0 ? 1 : (x * factorial(x - 1) )
factorial(5) == 120
```

And work well with arrays:
```lavendeux
sum(a) = !(is_empty(a)) ? pop(a) + sum(a) : 0
sum([10, 10, 11])
```

Extensions are described in detail in chapter 4.

A full list of available stdlib functions can be found in `section 5.2`

### 3.4 Loops and Conditionals

Conditionals are available as ternary expressions: `condition ? if_true : if_false`. Short-circuit evaluation is used in evaluation of these expressions:
```lavendeux
a = [1, 2, 3]
is_empty(a) ? [1] : a

is_empty(a) ? (del a) : a
len(a) == 3
```

Loops are available as mapping expressions over compound values, for example:
```lavendeux
for c in "hello world" do c + c
// = [hh, ee, ll, ll, oo,   , ww, oo, rr, ll, dd]

// You do not need to capture the count variable
for 0..10 do 5
// = [5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5]

a = [1, 2, 3]
for a do '!'
// [!, !, !]
```

## Chapter 4 - Extensions

Lavendeux can be extended at runtime using JS extensions loaded with `Lavendeux::load_extension(filename)`

Here is an example of a very simple extension that adds one function, and one decorator.
The name, author, and version calls are optional:
```js
lavendeuxExtensionName('Simple Extension');
lavendeuxExtensionAuthor('@anon');
lavendeuxExtensionVersion('1.0.0');

/**
 * Function adds the 2 operands and returns the result as an integer
 * Usage: add(<number>, <number>)
 * Can be called from the lavendeux parser
 * 
 * Accepts any numeric type, but the return type will always be cooerced to an integer
 */
lavendeuxFunction('add', (l, r) => l + r, {
    arguments: [lavendeuxType.Numeric, lavendeuxType.Numeric],
    returns: lavendeuxType.Int
});

/**
 * Formats an integer as a hex color code
 *  Usage: <number> @usd
 * Can be called from the lavendeux parser
 */
lavendeuxDecorator(
    'colour',
    (input) => `#${(input & 0x00FFFFFF).toString(16).padEnd(6, '0')}`,
    lavendeuxType.Int
);
```

## Chapter 5 - Appendix

### 5.1 Operator Reference

`+`     Numeric addition, string/array/object concatenation  
`-`     Numeric subtraction, string/array/object search and remove  
`*`     Numeric multiplication  
`/`     Numeric division    
`%`     Numeric modulo  
`**`    Numeric exponentiation  

`&`     Bitwise AND  
`|`     Bitwise OR  
`^`     Bitwise XOR  
`<<`    Bitwise left shift  
`>>`    Bitwise right shift  
`~`     Bitwise NOT  
  
`&&`    Boolean AND (short-circuiting)  
`||`    Boolean OR (short-circuiting)  
`!`     Boolean NOT  
`<`     Less than  
`<=`    Less than or equal to  
`>`     Greater than  
`>=`    Greater than or equal to  
`==`    Equal to  
`!=`    Not equal to  

### 5.2 stdlib Reference

Below is a full list of the functions and decorators built into the standard library

#### arrays

chunks(array, int) -> array : Splits the given array into chunks of the given size  
deque(array) -> any : Removes and returns the first element of the given array  
enque(array, any) -> array : Adds the given element to the beginning of the given array, then returns the array  
first(array) -> any : Returns the first element of the given array  
insert(compound, int, any) -> any : Inserts the given element at the given index in the given array or object  
is_empty(any) -> bool : Returns true if the given array or object is empty  
keys(object) -> array : Returns the keys of the given object  
last(array) -> any : Returns the last element of the given array  
len(any) -> int : Returns the length of the given array or object  
merge(compound, compound) -> compound : Merges the given arrays or objects  
pop(array) -> any : Removes and returns the last element of the given array  
push(array, any) -> array : Adds the given element to the end of the given array, then returns the array  
remove(compound, int) -> any : Removes the element at the given index from the given array or object and returns it  
split(array, int) -> array : Splits the given array at the given index  
values(object) -> array : Returns the values of the given object  
  
#### decorators  
  
@aud(any) -> string : Interprets a number as a AUD amount  
@bin(any) -> string : Base 2 number formatting, such as 0b101  
@bool(any) -> string : Format a value as a boolean  
@cad(any) -> string : Interprets a number as a CAD amount  
@cny(any) -> string : Interprets a number as a CNY amount  
@eur(any) -> string : Interprets a number as a Euro amount  
@float(any) -> string : Formats a number as a floating point number  
@gbp(any) -> string : Interprets a number as a GBP amount  
@hex(any) -> string : Base 16 number formatting, such as 0xFF  
@inr(any) -> string : Interprets a number as a INR amount  
@int(any) -> string : Format a number as an integer  
@jpy(any) -> string : Interprets a number as a JPY amount  
@oct(any) -> string : Base 8 number formatting, such as 0o77  
@ordinal(any) -> string : Interprets an integer as an ordinal number  
@percent(any) -> string : Interprets a number as a percentage  
@roman(any) -> string : Interprets an integer as a roman numeral  
@rub(any) -> string : Interprets a number as a RUB amount  
@sci(any) -> string : Scientific notation formatting, such as 1.2e3  
@usd(any) -> string : Interprets a number as a USD amount  
@utc(any) -> string : Interprets an integer as a timestamp, and formats it in UTC standard  
  
#### dev  
  
choose(array) -> any : Returns a random element from a given array  
md5(string) -> string : Returns the md5 hash of a given string  
rand(int, int) -> any : With no arguments, return a float from 0 to 1. Otherwise return an integer from 0 to m, or m to n  
sha256(string) -> string : Returns the sha256 hash of a given string  
sha512(string) -> string : Returns the sha512 hash of a given string  
tail(string, int) -> float : Returns the last [lines] lines from a given file  
time() -> float : Returns a unix timestamp for the current system time  
  
#### math  
  
abs(numeric) -> float : Returns the absolute value of a number  
ceil(numeric) -> float : Rounds a number up to the nearest integer  
floor(numeric) -> float : Rounds a number down to the nearest integer  
ln(numeric) -> float : Returns the natural logarithm of a number  
log(numeric, numeric) -> float : Returns the logarithm of a number with a given base  
log10(numeric) -> float : Returns the base 10 logarithm of a number  
max(compound) -> float : Returns the largest value in a set  
min(compound) -> float : Returns the smallest value in a set  
root(numeric, numeric) -> float : Returns the nth root of a number  
round(numeric, int) -> float : Rounds a number to a given precision  
sqrt(numeric) -> float : Returns the square root of a number  
  
#### network  
  
add_api(string, any) -> string : Registers an API. Accepts a string, or an object with the properties [ url, headers, description, examples, auth_key]  
api_key(string, string) -> string : Adds an API key for an API  
chatgpt(string) -> string : Calls the ChatGPT API  
del_api(string) -> string : Unregisters an API  
get(string, object) -> string : Performs an HTTP GET request  
get_api(string, string) -> string : Calls an API endpoint using GET  
list_api() -> object : Returns a list of registered APIs  
post(string, string, object) -> string : Performs an HTTP POST request  
post_api(string, string, string) -> string : Calls an API endpoint using POST  
resolve(string) -> string : Resolves a hostname to an IP address  
  
#### string  
  
atob(string) -> string : Returns a base64-encoded string  
btoa(string) -> string : Returns a base64-decoded string  
lowercase(string) -> string : Returns a lowercase version of a string  
prettyjson(any) -> string : Returns a pretty-printed JSON string  
replace(string, string, string) -> string : Replaces all instances of a substring with another substring  
trim(string) -> string : Removes whitespace from the beginning and end of a string  
trim_end(string) -> string : Removes whitespace from the end of a string  
trim_start(string) -> string : Removes whitespace from the beginning of a string  
uppercase(string) -> string : Returns an uppercase version of a string  
urldecode(string) -> string : Returns a URL-decoded string  
urlencode(string) -> string : Returns a URL-encoded string  
  
#### system  
  
add_extension(string) -> string : Adds a JavaScript extension to the interpreter  
assign(string, any) -> any : Assigns a variable in the current scope  
assign_global(string, any) -> any : Assigns a variable in the top-level scope  
eval(string) -> any : Evaluates a string as a Lavendeux expression and returns the result  
eval_file(string) -> any : Evaluates a file as a Lavendeux expression and returns the result  
js(string) -> any : Executes a JavaScript expression and returns the result  
remove_extension(string) -> string : Removes a JavaScript extension from the interpreter  
state() -> object : Returns the currently defined variables  
typeof(any) -> string : Returns the type of its input  
  
#### trig  
  
acos(numeric) -> float : Calculate the acos of n  
acosh(numeric) -> float : Calculate the acosh of n  
asin(numeric) -> float : Calculate the asin of n  
asinh(numeric) -> float : Calculate the asinh of n  
atan(numeric) -> float : Calculate the atan of n  
atanh(numeric) -> float : Calculate the atanh of n  
cos(numeric) -> float : Calculate the cos of n  
cosh(numeric) -> float : Calculate the cosh of n  
sin(numeric) -> float : Calculate the sin of n  
sinh(numeric) -> float : Calculate the sinh of n  
tan(numeric) -> float : Calculate the tan of n  
tanh(numeric) -> float : Calculate the tanh of n  
to_degrees(numeric) -> float : Converts radians to degrees  
to_radians(numeric) -> float : Converts degrees to radians  