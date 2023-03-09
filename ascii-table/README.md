# Integrating Munit with Replit for C Unit Testing

This template offers an example of how to integrate Munit ("micro-unit")[https://nemequ.github.io/munit/] to provide automated unit tests for C code.

In this example tests are written in the **tests.c** file which is compiled as normal.

The unit tests can be invoked (in the example) via **./tests**

Built with [Munit 5/11/20](https://nemequ.github.io/munit/).

## Sample Tests

Examine _tests.c_ for sample tests.  

# HOW TO INTEGRATE WITH YOUR EXISTING C PROJECT

## Copy the munit folder to your Replit. 
  Or download those from [https://nemequ.github.io/munit/] to get the latest version

## Copy the Makefile to your Replit. 
## Ensure that the project "fits" this Makefile
* _main()_ is in _main.c_
* No "functional" project file called _tests.c_ (this is reserved for unit tests)
* no code in subdirectories (modify _Makefile_ if needed)

## Examine tests.c for sample tests.

## type ./tests (or "make test")

## Note
This Makefile also enables all compiler warnings. Because.

# COMMENTARY:
  The tests[] and suite structures are a bit heavyweight for "just a few simple tests".
  But they open the door to much more complex (and industry-standard) test cases because
  of their support for setup, teardown, PRN, etc.
