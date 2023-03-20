# Integrating Munit with Replit for C Unit Testing

This template offers an example of how to integrate Munit ("micro-unit")[https://nemequ.github.io/munit/] to provide automated unit tests for C code.

In this example tests are written in the **tests.c** file which is compiled as normal.

The unit tests can be invoked (in the example) via **./tests** but **make test** is preferred.

Built with [Munit 5/11/20](https://nemequ.github.io/munit/).

## How to run tests

_make test_ is preferred.

Can do _./tests_ if you must.

## Sample Tests

Examine _tests.c_ for sample tests.  

# HOW TO INTEGRATE WITH YOUR EXISTING C PROJECT

## Copy the munit folder to your Replit. 
  Or download those from [github](https://nemequ.github.io/munit/) to get the latest version

## Copy the Makefile to your Replit. 
## Ensure that the project "fits" this Makefile
* _main()_ is in _main.c_
* No "functional" project file called _tests.c_ (this is reserved for unit tests)
* no code in subdirectories (modify [Makefile](https://replit.com/@bjmckenz/C-Template-Using-Munit-for-Unit-Tests#Makefile) if needed)

## Examine tests.c for sample tests.

## type ./tests (or "make test")

## Makefile features
This Makefile also enables all compiler warnings. Because.

The Makefile for this template supports the following specific targets:

* main
  * The application executable ("main"), built from main.cp.
  
* debug
  * A debug version of the application ("main-debug") with optimization turned off and debug symbols included.

* tests
  * A unit-testing executable that links with all application code (save that from main.cp).

* test
  * Execute the unit-testing executable, building it as needed.

* clean
  * Remove all intermediate and final products

* all
  * Produces the main, debug, and tests targets.

# COMMENTARY:
  The tests[] and suite structures are a bit heavyweight for "just a few simple tests".
  But they open the door to much more complex (and industry-standard) test cases because
  of their support for setup, teardown, PRN, etc.
