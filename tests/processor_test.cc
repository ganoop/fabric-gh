

#include "src/processor.h"
#include "src/isa.pb.h"
#include "src/tools.h"
#include "gtest/gtest.h"

using namespace fabric;
namespace
{
    TEST(Processor, LoadStore)
    {
        const int INP_ADDR = 5;
        const int OUT_ADDR = 23;
        fabric::VPUProgram prog;

        VPUInstruction *ld = prog.add_instr();
        ld->set_op(VPU_OP_LD);
        ld->add_inputs()->set_addr(INP_ADDR);
        ld->add_outputs()->set_reg(3);

        VPUInstruction *st = prog.add_instr();
        st->set_op(VPU_OP_ST);
        st->add_inputs()->set_reg(3);
        st->add_outputs()->set_addr(OUT_ADDR);

        Processor proc;
        proc.sram_[INP_ADDR] = 42;
        proc.run_program(prog);

        ASSERT_EQ(proc.sram_[OUT_ADDR], 42);
    }

    TEST(Processor, Branch)
    {
        const int OUT_ADDR = 23;
        fabric::VPUProgram prog;

        VPUInstruction *r1 = prog.add_instr();
        r1->set_op(VPU_OP_SET);
        r1->add_inputs()->set_value(1);
        r1->add_outputs()->set_reg(1); // R1 has value 1

        VPUInstruction *r3 = prog.add_instr();
        r3->set_op(VPU_OP_SET);
        r3->add_inputs()->set_value(5);
        r3->add_outputs()->set_reg(3); // R3 has value 5

        VPUInstruction *r4 = prog.add_instr();
        r4->set_op(VPU_OP_SET);
        r4->add_inputs()->set_value(5);
        r4->add_outputs()->set_reg(4); // R4 has value 5

        VPUInstruction *r5 = prog.add_instr();
        r5->set_op(VPU_OP_SET);
        r5->add_inputs()->set_value(10);
        r5->add_outputs()->set_reg(5); // R5 has value 10

        VPUInstruction *b1 = prog.add_instr();
        b1->set_op(VPU_OP_BEQ);
        b1->add_inputs()->set_reg(3);
        b1->add_inputs()->set_reg(4);
        b1->add_outputs()->set_label("next");

        VPUInstruction *a1 = prog.add_instr();
        a1->set_op(VPU_OP_ADD);
        a1->add_inputs()->set_reg(5);
        a1->add_inputs()->set_reg(1);
        a1->add_outputs()->set_reg(1);

        VPUInstruction *b2 = prog.add_instr();
        b2->set_op(VPU_OP_BEQ);
        b2->add_inputs()->set_reg(4);
        b2->add_inputs()->set_reg(5);
        b2->add_outputs()->set_reg(1);
        b2->add_outputs()->set_label("done");
        b2->set_label("next");

        VPUInstruction *a2 = prog.add_instr();
        a2->set_op(VPU_OP_ADD);
        a2->add_inputs()->set_reg(4);
        a2->add_inputs()->set_reg(1);
        a2->add_outputs()->set_reg(1);

        VPUInstruction *st = prog.add_instr();
        st->set_op(VPU_OP_ST);
        st->add_inputs()->set_reg(1);
        st->add_outputs()->set_addr(OUT_ADDR);
        st->set_label("done");

        Processor proc;

        proc.run_program(prog);

        ASSERT_EQ(proc.sram_[OUT_ADDR], 6);
    }

    TEST(Processor, AddP)
    {
        const int P_ADDR = 0;
        const int OUT_ADDR = 16;
        fabric::VPUProgram prog;

        VPUInstruction *ldp = prog.add_instr();
        ldp->set_op(VPU_OP_LDP);
        ldp->add_inputs()->set_addr(P_ADDR);

        VPUInstruction *r1 = prog.add_instr();
        r1->set_op(VPU_OP_SET);
        r1->add_inputs()->set_value(23);
        r1->add_outputs()->set_reg(1); // R1 has value 1

        VPUInstruction *a1 = prog.add_instr();
        a1->set_op(VPU_OP_ADDP);
        a1->add_inputs()->set_reg(1);
        a1->add_inputs()->set_reg(1);
        a1->add_outputs()->set_reg(3);

        VPUInstruction *st = prog.add_instr();
        st->set_op(VPU_OP_ST);
        st->add_inputs()->set_reg(3);
        st->add_outputs()->set_addr(OUT_ADDR);

        Processor proc;
        proc.sram_[P_ADDR] = 37;
        proc.run_program(prog);

        ASSERT_EQ(proc.sram_[OUT_ADDR], 9);
    }

    TEST(Processor, Subroutine)
    {
        const int INP_ADDR = 5;
        const int OUT_ADDR = 23;
        fabric::VPUProgram prog;

        VPUInstruction *r1 = prog.add_instr();
        r1->set_op(VPU_OP_SET);
        r1->add_inputs()->set_value(1);
        r1->add_outputs()->set_reg(1); // R1 has value 1

        VPUInstruction *j = prog.add_instr();
        j->set_op(VPU_OP_JAL);
        j->add_inputs()->set_label("call_add");
        j->add_outputs()->set_reg(10);

        VPUInstruction *st = prog.add_instr();
        st->set_op(VPU_OP_ST);
        st->add_inputs()->set_reg(1);
        st->add_outputs()->set_addr(OUT_ADDR);

        VPUInstruction *r5 = prog.add_instr();
        r5->set_op(VPU_OP_SET);
        r5->add_inputs()->set_value(100);
        r5->add_outputs()->set_reg(5); // R5 has value 100

        VPUInstruction *ret1 = prog.add_instr();
        ret1->set_op(VPU_OP_RET);
        ret1->add_inputs()->set_reg(5); // exit from program

        VPUInstruction *a1 = prog.add_instr();
        a1->set_op(VPU_OP_ADD);
        a1->add_inputs()->set_reg(1);
        a1->add_inputs()->set_reg(1);
        a1->add_outputs()->set_reg(1);
        a1->set_label("call_add");

        VPUInstruction *ret2 = prog.add_instr();
        ret2->set_op(VPU_OP_RET);
        ret2->add_inputs()->set_reg(10);

        Processor proc;
        proc.run_program(prog);

        ASSERT_EQ(proc.sram_[OUT_ADDR], 2);
    }

} // namespace