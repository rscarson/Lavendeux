export const HelpData = {
    "basic_syntax": [
        {"str": "help\\basic_syntax\\intro_1a"},
        {"str": "help\\basic_syntax\\intro_1b"},
        {"code": "\n\
            FLAG_FEATURE1 = 0b10000000\n\
            FLAG_FEATURE2 = 0b01000000\n\
            FLAG_FEATURE3 = 0b00100000\n\
            has_flag(input, flag) = (input & flag) == flag\n\
        "},
        
        {"str": "help\\basic_syntax\\intro_2"},
        {"code": "\n\
            bits = FLAG_FEATURE1 | FLAG_FEATURE2 @hex\n\
            has_flag(bits, FLAG_FEATURE1)\n\
            has_flag(bits, FLAG_FEATURE3)\n\
        "},

        {"hr": true},

        {"str": "help\\basic_syntax\\operators_1"},
        {"str": "help\\basic_syntax\\operators_2"},
        {"code": "\n\
            sqrt(16) ** 2 == 16     // Exponentiation\n\
            2 * 3 == 6              // Multiplication\n\
            2 << 1 == 4             // Bitwise shift\n\
            0x2 ^ 0x3 == 0x1        // Bitwise XOR\n\
            [1, 2] + 2 == [3, 4]    // Array OPs\n\
        "},
        
        {"str": "help\\basic_syntax\\types"},
        {"code": "\n\
            $1,000 + ¥1,000.00 == 2,000     // Currencies\n\
            5.6e+7 - .6E7 + .2e-3           // Sci notation\n\
            1 + 1.2 + .3 == 2.5             // Floats and integers\n\
            5 > 3 == true                   // Boolean\n\
            concat('foo', 'bar')            // String\n\
            [1, 2, 'test']                  // Arrays\n\
            {'key': 1, 2: 'value'}          // Objects\n\
        "},
    ],

    "features": [
        {"str": "help\\features\\functions"},
        {"code": "\n\
            f(x) = 3x**2 + 2x + 1; f(3) == 34\n\
            format_name(first, last) = concat(first, \" \", last)\n\
            x = [1, 2, 3, 4, 5]; pop(x) == 5; len(x) == 4\n\
        "},

        {"hr": true},

        {"str": "help\\features\\variables"},
        {"code": "\n\
        float_v = 0.02\n\
        int_v = 7\n\
        string_v = \"test\"\n\
        \n\
        float_v + int_v == 7.02\n\
        float_v + string_v == \"0.02test\"\n\
        "},

        {"hr": true},

        {"str": "help\\features\\decorators"},
        {"code": "\n\
            /* 0xFF */ 15 @hex\n\
            /* $20,000.00 */ 20000 @usd\n\
            /* 0b11 */ 3 @bin\n\
        "},

        {"hr": true},

        {"str": "help\\features\\arrays_and_objects"},
        {"code": "\n\
            arr = [1, 2.0, 'test', false]\n\
            obj = {0: 'test', 'test': 1, false:true}\n\
        "},
    ],

    "datatypes": [
        {"str": "help\\datatypes\\intro"},
        {"code": "\n\
            1 != 1.2 && 1 == 1.0\n\
            \"1\" == 1\n\
            0 == false\n\
            [1] == 1\n\
            [1] == {0: 1}\n\
        "},

        {"hr": true},
        
        {"str": "help\\datatypes\\list_1"},
        {"str": "help\\datatypes\\list_2"},
        {"code": "\n\
            /* Integers */  1000000; 1,000,000\n\
            /* Floats */    0.2; .2, 2.5e-1; 2.5e+1; 2e4; $1; $1.20\n\
            /* Booleans */  false; true\n\
            /* Strings */   'test'; 'test\\''; \"test\\n\"\n\
            /* Arrays */    []; [1, 'test']\n\
            /* Objects */   {}; {0: 1}; {'test': false}\n\
        "},
    ],

    "operators": [
        {"str": "help\\operators\\intro"},

        {"hr": true},
        {"str": "help\\operators\\bitwise"},
        {"code": "\n\
            /* AND */   0x0F & 0x0A == 0x0A\n\
            /* OR */    0x0F | 0xAF == 0xAF\n\
            /* XOR */   0x0F ^ 0x0A == 0x05\n\
            /* SHIFT */ 0x0F << 1 == 0x1E; 0x0F >> 1 == 0x07\n\
            /* NOT */   ~0x0A == 0x05\n\
        "},

        {"hr": true},
        {"str": "help\\operators\\boolean"},
        {"code": "\n\
            /* AND */  true && true == true\n\
            /* OR */   true || false == true\n\
            /* EQ */   5 == 5; 3 != 5\n\
            /* GT */   3 < 5; 3 <= 3\n\
            /* LT */   5 > 3; 3 >= 3\n\
        "},

        {"hr": true},
        {"str": "help\\operators\\math"},
        {"code": "\n\
            /* ADD */        1 + 2 + 3 == 6\n\
            /* SUB */        3 - 2 - 1 == 0; -2; -(3!)\n\
            /* MUL */        5 * 3 == 15; x=2; 2x == 4; (2)(3) == 6\n\
            /* DIV */        3 / 2.0 == 1.5\n\
            /* POW */        2**4 == 16\n\
            /* MODULO */     3 % 2 == 1\n\
            /* FACTORIAL */  4! == 24\n\
        "},

        {"hr": true},
        {"str": "help\\operators\\other"},
        {"code": "\n\
            x = 5\n\
            f(x) = 5x\n\
            x=[1,2]; x[0] == 1\n\
            y={'test': 4}; y['test'] == 4\n\
            \n\
            len(x) ? true : false\n\
            true ? 1 : 2\n\
        "},
    ],

    "functions": [
        
    ],

    "decorators": [],

    "extensions": []
};