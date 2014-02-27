Cero Programming Language
=========================

Cero is a slightly modified variant of C++. It compiles cero files into C++ and then builds using the C++ compiler. It is fully compatible with C/C++ code. Cero requires a C++11 compiler. Currently it works only on Linux.

Cero has the following modifications compared to C++:
* No semicolons
* No curly braces
* No parentheses after for, while, if, switch
* Simplified for loops (for i = 0..5)
* No header files 
* No forward declarations
* Variable creation simplfied: "auto i = 0" can be written as "i := 0"
* Simplified build system. Makefiles are not needed.

The cero compiler is licensed under the BSD license.
