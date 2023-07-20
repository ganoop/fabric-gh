
#ifndef PADD_H
#define PADD_H
#include "src/isa.pb.h"

using namespace fabric;

class PAdd
{
public:
    void generate(VPUProgram *prog)
    {
        // Assume input is in SRAM:
        //   0: prime number
        //   1: Curve A param
        //   2: Curve B param
        //   3: The value of EC-Zero (if x or y is this, the point is zero)
        //   4: EC x1 coordinate
        //   5: EC y1 coordinate
        //   6: EC x2 coordinate
        //   7: EC y2 coordinate

        // Output should be placed:
        //   10: EC result x
        //   11: EC result y

        // TODO: assemble your instructions into prog to add (x1, y1) to (x2, y2)
    }
};

#endif