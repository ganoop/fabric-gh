# Fabric Compiler SWE Assessment

## Problem
We need a framework for generating assembly/machine code for our custom cryptograph accelerator from higher-level kernels.  We have a preliminary architecture and ISA defined that is similar to RISC-V with some extensions, which you can use to implement some core functionality. 

See [the isa.proto file](src/isa.proto) for instruction definitions, we're using protobuf to represent instructions because they're structured, typed, and portable (cross-language).  Our architecture has 32 64-bit registers and a scratch-pad SRAM.  We're making addressing simple here with 64-bit/8-byte word addressing (e.g. address 2 is 16 bytes in, not 2 bytes in).

### Step 1
Most of our core operations use Prime Field arithmetic (everything is modulo some prime). While we can implement add, subtract, and multiply in hardware
and have instructions for them (ADDP, SUBP, MULP), division in prime fields
is only defined by calculating the [multiplicative inverse](https://en.wikipedia.org/wiki/Finite_field_arithmetic#Multiplicative_inverse) in software.  Implement the assembly in [src/mult_inverse.h](src/mult_inverse.h) that calculates the inverse for any input. While our test cases use small primes for readability, note that real world examples will use very large numbers, therefore lookup-table or brute force approaches will not be sufficient. The tests in
[tests/mult_inverse_test.cc] will test these, you can run all tests with `./run.sh`. (it also runs tests for Step 2, which you can ignore for now).  Included
is a unit test that simply prints out your implemented program, for convenience.
Tests are SRAM-to-SRAM (they load values into SRAM, run your program, and check
that the output in a particular address is correct).

### Step 2
One of the fundamental and critical-path operations to some of our algorithms is [Elliptic Curve Addition](https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication).  Implement the assembly to add two EC points together in [src/padd.h](src/padd.h).  The tests in [tests/padd_test.cc](tests/padd_test.cc) will test these.  You will likely have use your multiplicative inverse for performing division (we can generally defer division by using projective coordinates if we're doing many EC operations in a row, but
eventually we have to divide anyway). Note that zero is a specially-defined point that we have to check for explicitly (by defining a value for it). Also
note that adding a point to itself uses different arithmetic than adding two
different points.

Performance in a hypothetically pipelined implementation is not a concern here,
just functionality. 
