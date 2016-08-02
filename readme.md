#Lavendeux [![Build Status](https://travis-ci.org/rscarson/Lavendeux.svg?branch=master)](https://travis-ci.org/rscarson/Lavendeux)

[Installer and binaries](https://github.com/rscarson/Lavendeux/releases) | [Syntax and usage help](https://github.com/rscarson/Lavendeux/wiki) | [Building and installing](https://github.com/rscarson/Lavendeux/wiki/Building-and-Installing)

A small calculator for programmers. Allows you to perform calculations from within any text editor using one keyboard shortcut - CRTL-Space

One expression per line, leading whitespace is preserved

How to use it
-------------
Copy the following block of text into your favourite text editor, highlight it and press CTRL-Space:

    x = 8 + 3e+2
    x = sqrt(16)
    x**3 @hex   

Or, alternatively, turn auto copy/paste off in the menu, and CTRL-Space will instead solve any expressions in your clipboard, and replace them with the answer

What it can do
--------------
- Values can be integers, floating point, scientific notation, binary (*0b01010*), octal (*0o132423*), or hexadecimal (*0x76A*)
- Set and use variables, names can be composed of alphanumeric characters and underscores (x = 5+5)
- Use decorators at the end of an expression to modify the format of the output (@int, @float, @bin, @hex, and several others)
- Supports english and french languages
- 
- Call any of these functions:
    - Ceiling round : ceil(in)
    - Floor round : floor(in)
    - Round to n decimal places : round(in, decimal places)
    - Absolute value : abs(in)
    - Tangent : tan(in)
    - Cosine : cos(in)
    - Sine : sin(in)
    - Arc tangent : atan(in)
    - Arc cosine : acos(in)
    - Arc sine : asin(in)
    - Hyperbolic tangent : tanh(in)
    - Hyperbolic cosine : cosh(in)
    - Hyperbolic Hyperbolic sine : sinh(in)
    - Base 10 log : log10(in)
    - Natural logarithm : ln(in)
    - Base n logarithm : log(in, base)
    - Square root : sqrt(in)
    - Nth root : root(in, base)
- Mathematical operators:
    - Add ( + )
    - Substract ( - )
    - Multiply ( * )
    - Divide ( / )
    - Exponentiation ( ** )
    - Modulus ( % )
    - Factorial ( ! )
- Binary operators:
    - And ( & )
    - Or ( | )
    - Xor ( ^ )
    - Not ( ~ )
    - Left shift ( < )
    - Right shift ( > )

## To do
- Linux and OSX versions
- Anti-trig functions (cotan, cosec, sec)
- Generating test sentences to automate parser tests
- Unit tests
- Extension support
