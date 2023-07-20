
#ifndef MULT_INVERSE_H
#define MULT_INVERSE_H
#include "src/isa.pb.h"

using namespace fabric;

class MultInverse
{
public:
    void generate(VPUProgram *prog)
    {
        // Assume input is in SRAM:
        //   0: prime number
        //   1: x

        // Output should be placed:
        //   10: inverse(x)

        // TODO: assemble your instructions into prog to
    }
};

#endif