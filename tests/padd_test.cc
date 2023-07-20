
#include "src/padd.h"
#include "src/processor.h"
#include "src/isa.pb.h"
#include "src/tools.h"
#include "gtest/gtest.h"

namespace
{
    TEST(PAdd, Print)
    {
        PAdd p;
        fabric::VPUProgram prog;
        p.generate(&prog);

        std::cout << program_to_str(prog) << std::endl;
    }

    TEST(PAdd, SomePoints)
    {
        PAdd p;
        fabric::VPUProgram prog;
        p.generate(&prog);

        int prime = 37;
        int a = 1;
        int b = 1;
        int zero = 0xffffffff;
        std::vector<std::tuple<std::pair<int, int>, std::pair<int, int>, std::pair<int, int>>> tests = {
            {
                {8, 22},
                {8, 22},
                {14, 24},
            },
            {
                {zero, zero},
                {6, 36},
                {6, 36},
            },
            {
                {2, 14},
                {36, 31},
                {27, 29},
            }};

        for (const auto &t : tests)
        {
            Processor proc;
            proc.sram_[0] = prime;
            proc.sram_[1] = a;
            proc.sram_[2] = b;
            proc.sram_[3] = zero;
            proc.sram_[4] = std::get<0>(t).first;
            proc.sram_[5] = std::get<0>(t).second;
            proc.sram_[6] = std::get<1>(t).first;
            proc.sram_[7] = std::get<1>(t).second;
            proc.run_program(prog);

            ASSERT_EQ(proc.sram_[10], std::get<2>(t).first);
            ASSERT_EQ(proc.sram_[11], std::get<2>(t).second);
        }
    }

} // namespace
