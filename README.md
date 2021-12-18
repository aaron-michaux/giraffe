# giraffe
Scanning dependencies

## TODO

 * We can use [https://github.com/gcc-mirror/gcc/tree/releases/gcc-11.2.0/gcc/testsuite/gcc.dg/cpp](https://github.com/gcc-mirror/gcc/tree/releases/gcc-11.2.0/gcc/testsuite/gcc.dg/cpp)
 * Run each testcase through gcc -E
 * Use some bash magic to turn this into a set of tests that can be run through catch

``` c++
# if     constant-expression NL
# ifdef  identifier NL
# ifndef identifier NL
# elif   constant-expression NL
# else   NL
# endif  NL

# include include-arg NL
# define identifier ... NL
# undef
# line pp-tokens
# error pp-tokens
# pramga p-tokens


__FILE__
__LINE__
__DATE__
__TIME__
__STDC__
__STDC_VERSION__
__STDC_HOSTED__
__cplusplus
__OBJC__
__ASSEMBLER__

constant-expression:
    '(' constant-expression ')'
    '!'? defined identifier

X > 3

#if __GNUC__ > 3 || \
    (__GNUC__ == 3 && (__GNUC_MINOR__ > 2 || \
                       (__GNUC_MINOR__ == 2 && \
                        __GNUC_PATCHLEVEL__ > 0))
or this

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)


"foo" "bar" => "foobar"

```

