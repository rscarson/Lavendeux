## Basic Syntax: 
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

## Features: 
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

## Datatypes: 
The example below shows the various representations of the available types:

```javascript
    1 != 1.2 && 1 == 1.0
    "1" == 1
    0 == false
    [1] == 1
    [1] == {0: 1}
```

---

Lavendeux provides the following types: integer, float, string, boolean, arrays, and objects. Any type can be cooerced into any other type - EXCEPT for numbers, which cannot be built from non-numeric types

Lavendeux is loosely typed - which means that most operations will accept most combinations of types. Below are some examples of type cooersion Lavendeux provides:
```javascript
    /* Integers */  1000000; 1,000,000
    /* Floats */    0.2; .2, 2.5e-1; 2.5e+1; 2e4; $1; $1.20
    /* Booleans */  false; true
    /* Strings */   'test'; 'test\''; "test"
    /* Arrays */    []; [1, 'test']
    /* Objects */   {}; {0: 1}; {'test': false}
```

## Operators: 
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
