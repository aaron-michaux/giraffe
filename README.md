# giraffe

Discover `module` and `#include` dependencies for a given file.

## Setting up a project configuration file.

To be useful, `giraffe` needs to know the include paths and
`#define` macros being using by your project's gcc installation.
This information can be discovered like so:

```
# Print out include paths, and #define macros.
# Use 'gcc' for C code. Don't forget to specify
# any additional command line options.
gcc -x c++ -dM -E -Wp,-v - </dev/null
```

Once the correct set of command line options is specified, save
the output to a configuration file that is later loaded into `giraffe`.

```
# Example, using C++20. Note use of 2>&1 to capture include paths.
gcc -x c++ -std=c++20 -dM -E -Wp,-v - </dev/null 1>g++-config.text 2>&1
```

## Running 

## Example Project

 * TODO, very minimal example Makefile
 * Example with `run.sh`

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
 
