
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

       //Planning to have design MIBuilder with static register mask
       //This is a leaf function. Using R8 as return register 
       //Rough registers convention: R1-R7 caller/callee, R10 link register
       //R8 as return register
       //R0=0, R22-1, R23-2, R20-R31 Global
       //R20: x
       //R21: P, P-2

       //BB1
       const int P_ADDR = 0;
       const int X_ADDR = 1;
       const int OUT_ADDR = 10;

       VPUInstruction *ldp = prog->add_instr();
       ldp->set_op(VPU_OP_LDP);
       ldp->add_inputs()->set_addr(P_ADDR);
       ldp->set_label("multinv");

       //Result in R8
       VPUInstruction *res = prog->add_instr();
       res->set_op(VPU_OP_SET);
       res->add_inputs()->set_value(1);
       res->add_outputs()->set_reg(8); // R8 initialized to 1

       //Load x into R20
       VPUInstruction *ldx = prog->add_instr();
       ldx->set_op(VPU_OP_LD);
       ldx->add_inputs()->set_addr(X_ADDR);
       ldx->add_outputs()->set_reg(20);

       VPUInstruction *one = prog->add_instr();
       one->set_op(VPU_OP_SET);
       one->add_inputs()->set_value(1);
       one->add_outputs()->set_reg(22); // R22 initialized to 1

       //if x == 1, just return 1
       VPUInstruction *cmpeq1 = prog->add_instr();
       cmpeq1->set_op(VPU_OP_BEQ);
       cmpeq1->add_inputs()->set_reg(20);
       cmpeq1->add_inputs()->set_reg(22);
       cmpeq1->add_outputs()->set_label("done");

       VPUInstruction *ld = prog->add_instr();
       ld->set_op(VPU_OP_LD);
       ld->add_inputs()->set_addr(P_ADDR);
       ld->add_outputs()->set_reg(21);

       VPUInstruction *two = prog->add_instr();
       two->set_op(VPU_OP_SET);
       two->add_inputs()->set_value(2);
       two->add_outputs()->set_reg(23); // R23 has value 2 

       //(Mod - 2) in register R20
       VPUInstruction *sub = prog->add_instr();
       sub->set_op(VPU_OP_SUB);
       sub->add_inputs()->set_reg(21);
       sub->add_inputs()->set_reg(23);
       sub->add_outputs()->set_reg(21);

       VPUInstruction *zero = prog->add_instr();
       zero->set_op(VPU_OP_SET);
       zero->add_inputs()->set_value(0);
       zero->add_outputs()->set_reg(0); // R0 has value 0 

       VPUInstruction *cmp = prog->add_instr();
       cmp->set_op(VPU_OP_BEQ);
       cmp->add_inputs()->set_reg(21);
       cmp->add_inputs()->set_reg(0);
       cmp->add_outputs()->set_label("done");

       //BB2 while loop ---------
       VPUInstruction *andp = prog->add_instr();
       andp->set_op(VPU_OP_AND);
       andp->add_inputs()->set_reg(21);
       andp->add_inputs()->set_reg(22);
       andp->add_outputs()->set_reg(13);
       andp->set_label("while");

       VPUInstruction *cmpand = prog->add_instr();
       cmpand->set_op(VPU_OP_BEQ);
       cmpand->add_inputs()->set_reg(13);
       cmpand->add_inputs()->set_reg(0);
       cmpand->add_outputs()->set_label("next");

       VPUInstruction *mulp = prog->add_instr();
       mulp->set_op(VPU_OP_MULP);
       mulp->add_inputs()->set_reg(8);
       mulp->add_inputs()->set_reg(20);
       mulp->add_outputs()->set_reg(8);

       //next: We don't have shift right insns - just divide by 2
       VPUInstruction *div = prog->add_instr();
       div->set_op(VPU_OP_DIV);
       div->add_inputs()->set_reg(21);
       div->add_inputs()->set_reg(23);
       div->add_outputs()->set_reg(21);
       div->set_label("next");

       VPUInstruction *mulpx = prog->add_instr();
       mulpx->set_op(VPU_OP_MULP);
       mulpx->add_inputs()->set_reg(20);
       mulpx->add_inputs()->set_reg(20);
       mulpx->add_outputs()->set_reg(20);


       VPUInstruction *cmpwhile = prog->add_instr();
       cmpwhile->set_op(VPU_OP_BEQ);
       cmpwhile->add_inputs()->set_reg(21);
       cmpwhile->add_inputs()->set_reg(0);
       cmpwhile->add_outputs()->set_label("done");

       VPUInstruction *ba = prog->add_instr();
       ba->set_op(VPU_OP_BEQ);
       ba->add_inputs()->set_reg(0);
       ba->add_inputs()->set_reg(0);
       ba->add_outputs()->set_label("while");

       VPUInstruction *st = prog->add_instr();
       st->set_op(VPU_OP_ST);
       st->add_inputs()->set_reg(8);
       st->add_outputs()->set_addr(OUT_ADDR);
       st->set_label("done");
   
       //VPUInstruction *ret = prog->add_instr();
       //ret->set_op(VPU_OP_RET);
       //ret->add_inputs()->set_reg(10);


    }
};

#endif
