# giraffe

Discover `module` and `#include` dependencies for a given file.

## Usage

## Example Project

## Limitations

 * Intended for `gcc` -- would not be hard to support clang.
 * Have to supply the installation path for system libraries.
 * No `sizeof` operator in constant expressions.
 * All constant expressions are treated as integers.

# TODO

 * Modules tests
 * Expressions tests
 * #if-then evaluation
 * #include evaluation: requires a new scanner
 
