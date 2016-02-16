#Lavendeux
[Installer and binaries](http://rscarson.github.io/Lavendeux/)

A small calculator for programmers. Allows you to perform calculations from within any text editor using one keyboard shortcut - CRTL-Space

One equation per line, start of line whitespace is preserved

##Syntax and supported functions

### Variables

    x = 5
    y = sqrt(x)
    x = x * y

###Mathematical operators:

- Add ( + )
- Substract ( - )
- Multiply ( * )
- Divide ( / )
- Exponentiation ( ** )
- Modulus ( % )
- Factorial ( ! )

###Binary operators:

- And ( & )
- Or ( | )
- Xor ( ^ )
- Not ( ~ )
- Left shift ( < )
- Right shift ( > )

## Functions

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


##Decorators

Add these to the end of an expression to modify the output format:

5+5 @\<decorator\>

- Unsigned (u, unsigned)
- Integer (i, int, integer)
- Floating point (f, float)
- Scientific notation (sci, scientific)
- Binary (b, bin, binary)
- Octal (o, oct, octal)
- Hexidecimal (h, hex, hexadecimal)

##Commandline options

**--help** : Show this message

**-h** : Short form of --help

**--config-path=\<path\>** : Use \<path\> as the configuration file

**-c=\<path\>** : Short form of --config-path

**--no-config** : Do not use a configuration file

**--angle-mode=\<degrees/radians\>** : Start in a given mode for angles

**-a=\<degrees/radians\>** : Short form of --angle-mode

**--silent-errors=\<on/off\>** : Start with silen errors on or off

**-s=\<on/off\>** : Short form of --silent-errors

**--language=\<en/fr\>** : Start in a given language

**-l=\<en/fr\>** : Short form of --language

## To do

- Linux and OSX versions
- Anti-trig functions (cotan, cosec, sec)
- Generating test sentences to automate parser tests
