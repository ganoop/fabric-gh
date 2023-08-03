
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
        //Rough Register convention: R1-R7 caller/callee, R10, R11 link register
        //R8-R9 return registers, R13-R19 temporaries, R20-R31 Global
        //R0->-0,R1>-X1,R2->Y1, R3>-X2, R4>-Y2, R5->P, R6->a,R7->m(slope),R12->zero
       //R20-> x argument to multinv , R12=zero 9xffffffff
        const int P_ADDR = 0;
        const int A_ADDR = 1;
        const int B_ADDR = 2;
        const int Z_ADDR = 3;
        const int X1_ADDR = 4;
        const int Y1_ADDR = 5;
        const int X2_ADDR = 6;
        const int Y2_ADDR = 7;
	const int INV_ARG_ADDR = 1;
	const int OUT_ADDR1 = 10;
	const int OUT_ADDR2 = 11;
        /*************************************************************
        NOTE: It is not good to keeping all incoming parameters into registers
        in the beginning of function. It increase live span. We doin't have 
        register presuure in the function. For simplicity, we have following
        convention. 
        **************************************************************/
        VPUInstruction *ldp = prog->add_instr();
        ldp->set_op(VPU_OP_LDP);
        ldp->add_inputs()->set_addr(P_ADDR);

        VPUInstruction *lda = prog->add_instr();
        lda->set_op(VPU_OP_LD);
        lda->add_inputs()->set_addr(A_ADDR);
        lda->add_outputs()->set_reg(6);

        VPUInstruction *ld = prog->add_instr();
        ld->set_op(VPU_OP_LD);
        ld->add_inputs()->set_addr(P_ADDR);
        ld->add_outputs()->set_reg(5);

        VPUInstruction *ldz = prog->add_instr();
        ldz->set_op(VPU_OP_LD);
        ldz->add_inputs()->set_addr(Z_ADDR);
        ldz->add_outputs()->set_reg(12);

        VPUInstruction *zero = prog->add_instr();
        zero->set_op(VPU_OP_SET);
        zero->add_inputs()->set_value(0);
        zero->add_outputs()->set_reg(0); 

        VPUInstruction *ldx1 = prog->add_instr();
        ldx1->set_op(VPU_OP_LD);
        ldx1->add_inputs()->set_addr(X1_ADDR);
        ldx1->add_outputs()->set_reg(1);

        VPUInstruction *ldy1 = prog->add_instr();
        ldy1->set_op(VPU_OP_LD);
        ldy1->add_inputs()->set_addr(Y1_ADDR);
        ldy1->add_outputs()->set_reg(2);

        VPUInstruction *ldx2 = prog->add_instr();
        ldx2->set_op(VPU_OP_LD);
        ldx2->add_inputs()->set_addr(X2_ADDR);
        ldx2->add_outputs()->set_reg(3);

        VPUInstruction *ldy2 = prog->add_instr();
        ldy2->set_op(VPU_OP_LD);
        ldy2->add_inputs()->set_addr(Y2_ADDR);
        ldy2->add_outputs()->set_reg(4);

        // if x1 or y1 equal zero 0xffffffff
        VPUInstruction *orp1 = prog->add_instr();
        orp1->set_op(VPU_OP_OR);
        orp1->add_inputs()->set_reg(1);
        orp1->add_inputs()->set_reg(2);
        orp1->add_outputs()->set_reg(13); 

        VPUInstruction *p1x = prog->add_instr();
        p1x->set_op(VPU_OP_ADD);
        p1x->add_inputs()->set_reg(3);
        p1x->add_inputs()->set_reg(0);
        p1x->add_outputs()->set_reg(8); 

        VPUInstruction *p1y = prog->add_instr();
        p1y->set_op(VPU_OP_ADD);
        p1y->add_inputs()->set_reg(4);
        p1y->add_inputs()->set_reg(0);
        p1y->add_outputs()->set_reg(9); 

        VPUInstruction *cmpor1 = prog->add_instr();
        cmpor1->set_op(VPU_OP_BEQ);
        cmpor1->add_inputs()->set_reg(13);
        cmpor1->add_inputs()->set_reg(12);
        cmpor1->add_outputs()->set_label("done");

        // if x2 or y2 equal zero 0xffffffff
        VPUInstruction *orp2 = prog->add_instr();
        orp2->set_op(VPU_OP_OR);
        orp2->add_inputs()->set_reg(3);
        orp2->add_inputs()->set_reg(4);
        orp2->add_outputs()->set_reg(13); 

        VPUInstruction *p2x = prog->add_instr();
        p2x->set_op(VPU_OP_ADD);
        p2x->add_inputs()->set_reg(1);
        p2x->add_inputs()->set_reg(0);
        p2x->add_outputs()->set_reg(8); 

        VPUInstruction *p2y = prog->add_instr();
        p2y->set_op(VPU_OP_ADD);
        p2y->add_inputs()->set_reg(2);
        p2y->add_inputs()->set_reg(0);
        p2y->add_outputs()->set_reg(9); 

        VPUInstruction *cmpor2 = prog->add_instr();
        cmpor2->set_op(VPU_OP_BEQ);
        cmpor2->add_inputs()->set_reg(13);
        cmpor2->add_inputs()->set_reg(12);
        cmpor2->add_outputs()->set_label("done");

        VPUInstruction *cmpx1x2 = prog->add_instr();
        cmpx1x2->set_op(VPU_OP_BEQ);
        cmpx1x2->add_inputs()->set_reg(1);
        cmpx1x2->add_inputs()->set_reg(3);
        cmpx1x2->add_outputs()->set_label("xsame");

        VPUInstruction *subpy = prog->add_instr();
        subpy->set_op(VPU_OP_SUBP);
        subpy->add_inputs()->set_reg(2);
        subpy->add_inputs()->set_reg(4);
        subpy->add_outputs()->set_reg(13);

        VPUInstruction *addpiy = prog->add_instr();
        addpiy->set_op(VPU_OP_ADDP);
        addpiy->add_inputs()->set_reg(13);
        addpiy->add_inputs()->set_reg(5);
        addpiy->add_outputs()->set_reg(14);

        VPUInstruction *subpx = prog->add_instr();
        subpx->set_op(VPU_OP_SUBP);
        subpx->add_inputs()->set_reg(1);
        subpx->add_inputs()->set_reg(3);
        subpx->add_outputs()->set_reg(15);

        VPUInstruction *addpix = prog->add_instr();
        addpix->set_op(VPU_OP_ADDP);
        addpix->add_inputs()->set_reg(15);
        addpix->add_inputs()->set_reg(5);
        addpix->add_outputs()->set_reg(20);

        VPUInstruction *j = prog->add_instr();
        j->set_op(VPU_OP_JAL);
        j->add_inputs()->set_label("call_multinv");
        j->add_outputs()->set_reg(10);

        VPUInstruction *mulp = prog->add_instr();
        mulp->set_op(VPU_OP_MULP);
        mulp->add_inputs()->set_reg(14);
        mulp->add_inputs()->set_reg(8);
        mulp->add_outputs()->set_reg(7);

        VPUInstruction *cmpa = prog->add_instr();
        cmpa->set_op(VPU_OP_BEQ);
        cmpa->add_inputs()->set_reg(0);
        cmpa->add_inputs()->set_reg(0);
        cmpa->add_outputs()->set_label("intersect");

//-----------------------------------------------------
       //Result of multinv in R8
       VPUInstruction *res = prog->add_instr();
       res->set_op(VPU_OP_SET);
       res->add_inputs()->set_value(1);
       res->add_outputs()->set_reg(8); // R8 initialized to 1
       res->set_label("call_multinv");

       //Loop Invariant in while loop as well
       VPUInstruction *one = prog->add_instr();
       one->set_op(VPU_OP_SET);
       one->add_inputs()->set_value(1);
       one->add_outputs()->set_reg(22); // R22 initialized to 1

       //if x == 1, just return 1
       VPUInstruction *cmpeq1 = prog->add_instr();
       cmpeq1->set_op(VPU_OP_BEQ);
       cmpeq1->add_inputs()->set_reg(20);
       cmpeq1->add_inputs()->set_reg(22);
       cmpeq1->add_outputs()->set_label("mi_done");

       //Loop invariant in while
       VPUInstruction *twom = prog->add_instr();
       twom->set_op(VPU_OP_SET);
       twom->add_inputs()->set_value(2);
       twom->add_outputs()->set_reg(23); // R23 has value 2

       //(Mod - 2) in register R20
       VPUInstruction *sub = prog->add_instr();
       sub->set_op(VPU_OP_SUB);
       sub->add_inputs()->set_reg(5);
       sub->add_inputs()->set_reg(23);
       sub->add_outputs()->set_reg(21);

       VPUInstruction *cmp = prog->add_instr();
       cmp->set_op(VPU_OP_BEQ);
       cmp->add_inputs()->set_reg(21);
       cmp->add_inputs()->set_reg(0);
       cmp->add_outputs()->set_label("mi_done");

       // while loop ---------
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

       VPUInstruction *mulpm = prog->add_instr();
       mulpm->set_op(VPU_OP_MULP);
       mulpm->add_inputs()->set_reg(8);
       mulpm->add_inputs()->set_reg(20);
       mulpm->add_outputs()->set_reg(8);

       //next: We don't have shift right insns - just divide by 2
       VPUInstruction *div = prog->add_instr();
       div->set_op(VPU_OP_DIV);
       div->add_inputs()->set_reg(21);
       div->add_inputs()->set_reg(23);
       div->add_outputs()->set_reg(21);
       div->set_label("next");

       VPUInstruction *mulpmix = prog->add_instr();
       mulpmix->set_op(VPU_OP_MULP);
       mulpmix->add_inputs()->set_reg(20);
       mulpmix->add_inputs()->set_reg(20);
       mulpmix->add_outputs()->set_reg(20);

       VPUInstruction *cmpwhile = prog->add_instr();
       cmpwhile->set_op(VPU_OP_BEQ);
       cmpwhile->add_inputs()->set_reg(21);
       cmpwhile->add_inputs()->set_reg(0);
       cmpwhile->add_outputs()->set_label("mi_done");

       VPUInstruction *ba = prog->add_instr();
       ba->set_op(VPU_OP_BEQ);
       ba->add_inputs()->set_reg(0);
       ba->add_inputs()->set_reg(0);
       ba->add_outputs()->set_label("while");

       VPUInstruction *ret = prog->add_instr();
       ret->set_op(VPU_OP_RET);
       ret->add_inputs()->set_reg(10);
       ret->set_label("mi_done");

        VPUInstruction *cmpy = prog->add_instr();
        cmpy->set_op(VPU_OP_BEQ);
        cmpy->add_inputs()->set_reg(2);
        cmpy->add_inputs()->set_reg(4);
        cmpy->add_outputs()->set_label("ysame");
        cmpy->set_label("xsame");

        //NOTE:x1 == x2 && y1 != y2 returm (P(mod) ,Zero(0xffffffff))
        VPUInstruction *ordiv0x = prog->add_instr();
        ordiv0x->set_op(VPU_OP_OR);
        ordiv0x->add_inputs()->set_reg(5);
        ordiv0x->add_inputs()->set_reg(5);
        ordiv0x->add_outputs()->set_reg(8); 

        VPUInstruction *ordiv0y = prog->add_instr();
        ordiv0y->set_op(VPU_OP_OR);
        ordiv0y->add_inputs()->set_reg(12);
        ordiv0y->add_inputs()->set_reg(12);
        ordiv0y->add_outputs()->set_reg(9); 

        VPUInstruction *cmpa1 = prog->add_instr();
        cmpa1->set_op(VPU_OP_BEQ);
        cmpa1->add_inputs()->set_reg(0);
        cmpa1->add_inputs()->set_reg(0);
        cmpa1->add_outputs()->set_label("done");
    
        //y1 == y2 && x1 == x2 Block for same point: double, we need tangent
        VPUInstruction *mulpx = prog->add_instr();
        mulpx->set_op(VPU_OP_MULP);
        mulpx->add_inputs()->set_reg(1);
        mulpx->add_inputs()->set_reg(1);
        mulpx->add_outputs()->set_reg(13);
        mulpx->set_label("ysame");

        VPUInstruction *three = prog->add_instr();
        three->set_op(VPU_OP_SET);
        three->add_inputs()->set_value(3);
        three->add_outputs()->set_reg(14); 

        VPUInstruction *mulpx3 = prog->add_instr();
        mulpx3->set_op(VPU_OP_MULP);
        mulpx3->add_inputs()->set_reg(13);
        mulpx3->add_inputs()->set_reg(14);
        mulpx3->add_outputs()->set_reg(15);

        //plus a
        VPUInstruction *num = prog->add_instr();
        num->set_op(VPU_OP_ADDP);
        num->add_inputs()->set_reg(15);
        num->add_inputs()->set_reg(6);
        num->add_outputs()->set_reg(16);

        VPUInstruction *two = prog->add_instr();
        two->set_op(VPU_OP_SET);
        two->add_inputs()->set_value(2);
        two->add_outputs()->set_reg(14); 

        VPUInstruction *denom = prog->add_instr();
        denom->set_op(VPU_OP_MULP);
        denom->add_inputs()->set_reg(2);
        denom->add_inputs()->set_reg(14);
        denom->add_outputs()->set_reg(20);

        VPUInstruction *jd = prog->add_instr();
        jd->set_op(VPU_OP_JAL);
        jd->add_inputs()->set_label("call_multinv");
        jd->add_outputs()->set_reg(10);

        //slope
        VPUInstruction *mulpd = prog->add_instr();
        mulpd->set_op(VPU_OP_MULP);
        mulpd->add_inputs()->set_reg(16);
        mulpd->add_inputs()->set_reg(8);
        mulpd->add_outputs()->set_reg(7);
    
        //-----------------Intersect
        VPUInstruction *mulpmx = prog->add_instr();
        mulpmx->set_op(VPU_OP_MULP);
        mulpmx->add_inputs()->set_reg(7);
        mulpmx->add_inputs()->set_reg(1);
        mulpmx->add_outputs()->set_reg(13);
        mulpmx->set_label("intersect");

        VPUInstruction *addpy = prog->add_instr();
        addpy->set_op(VPU_OP_ADDP);
        addpy->add_inputs()->set_reg(2);
        addpy->add_inputs()->set_reg(5);
        addpy->add_outputs()->set_reg(17);
    
        VPUInstruction *yminusmx = prog->add_instr();
        yminusmx->set_op(VPU_OP_SUBP);
        yminusmx->add_inputs()->set_reg(17);
        yminusmx->add_inputs()->set_reg(13);
        yminusmx->add_outputs()->set_reg(14);
    
        VPUInstruction *mulpmm = prog->add_instr();
        mulpmm->set_op(VPU_OP_MULP);
        mulpmm->add_inputs()->set_reg(7);
        mulpmm->add_inputs()->set_reg(7);
        mulpmm->add_outputs()->set_reg(15);
    
        VPUInstruction *subpx1 = prog->add_instr();
        subpx1->set_op(VPU_OP_SUBP);
        subpx1->add_inputs()->set_reg(5);
        subpx1->add_inputs()->set_reg(1);
        subpx1->add_outputs()->set_reg(16);
    
        VPUInstruction *subpx2 = prog->add_instr();
        subpx2->set_op(VPU_OP_SUBP);
        subpx2->add_inputs()->set_reg(5);
        subpx2->add_inputs()->set_reg(3);
        subpx2->add_outputs()->set_reg(17);
    
        VPUInstruction *addpx = prog->add_instr();
        addpx->set_op(VPU_OP_ADDP);
        addpx->add_inputs()->set_reg(16);
        addpx->add_inputs()->set_reg(17);
        addpx->add_outputs()->set_reg(18);
    
        VPUInstruction *addrx = prog->add_instr();
        addrx->set_op(VPU_OP_ADDP);
        addrx->add_inputs()->set_reg(15);
        addrx->add_inputs()->set_reg(18);
        addrx->add_outputs()->set_reg(8);
    
        //slope
        VPUInstruction *mulpi = prog->add_instr();
        mulpi->set_op(VPU_OP_MULP);
        mulpi->add_inputs()->set_reg(7);
        mulpi->add_inputs()->set_reg(8);
        mulpi->add_outputs()->set_reg(13);
    
        VPUInstruction *subp1 = prog->add_instr();
        subp1->set_op(VPU_OP_SUBP);
        subp1->add_inputs()->set_reg(5);
        subp1->add_inputs()->set_reg(13);
        subp1->add_outputs()->set_reg(15);

        VPUInstruction *subp2 = prog->add_instr();
        subp2->set_op(VPU_OP_SUBP);
        subp2->add_inputs()->set_reg(5);
        subp2->add_inputs()->set_reg(14);
        subp2->add_outputs()->set_reg(16);
    
        VPUInstruction *addpry = prog->add_instr();
        addpry->set_op(VPU_OP_ADDP);
        addpry->add_inputs()->set_reg(15);
        addpry->add_inputs()->set_reg(16);
        addpry->add_outputs()->set_reg(9);
      
        VPUInstruction *stx = prog->add_instr();
        stx->set_op(VPU_OP_ST);
        stx->add_inputs()->set_reg(8);
        stx->add_outputs()->set_addr(OUT_ADDR1);
        stx->set_label("done");

        VPUInstruction *sty = prog->add_instr();
        sty->set_op(VPU_OP_ST);
        sty->add_inputs()->set_reg(9);
        sty->add_outputs()->set_addr(OUT_ADDR2);

        //VPUInstruction *ret = prog->add_instr();
        //ret->set_op(VPU_OP_RET);
        //ret->add_inputs()->set_reg(10);
    
    }
};

#endif
