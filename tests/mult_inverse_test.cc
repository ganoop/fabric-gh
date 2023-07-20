

#include "src/processor.h"
#include "src/mult_inverse.h"
#include "src/isa.pb.h"
#include "src/tools.h"
#include "gtest/gtest.h"

namespace
{
    TEST(MultInverse, Print)
    {
        MultInverse p;
        fabric::VPUProgram prog;
        p.generate(&prog);

        std::cout << program_to_str(prog) << std::endl;
    }

    TEST(MultInverse, SomeNumbers)
    {
        MultInverse p;
        fabric::VPUProgram prog;
        p.generate(&prog);

        int P = 37;
        std::vector<std::pair<int, int>> tests = {
            {4, 28},
            {7, 16},
            {36, 36},
            {23, 29}};

        for (const auto &t : tests)
        {
            Processor proc;
            proc.sram_[0] = P;
            proc.sram_[1] = t.first;
            proc.run_program(prog);

            ASSERT_EQ(proc.sram_[10], t.second);
        }
    }

} // namespace