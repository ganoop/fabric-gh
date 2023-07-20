
#include "src/processor.h"
#include "src/tools.h"

class Exec_LDP : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->p_ = proc->sram_[i.inputs().at(0).addr()];
    }
};

template <typename Op>
class Exec_Binary : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = Op()(proc->regs_[i.inputs().at(0).reg()], proc->regs_[i.inputs().at(1).reg()]);
    }
};

class Exec_ADD : public Exec_Binary<std::plus<Word>>
{
};

class Exec_SUB : public Exec_Binary<std::minus<Word>>
{
};

class Exec_MUL : public Exec_Binary<std::multiplies<Word>>
{
};

class Exec_DIV : public Exec_Binary<std::divides<Word>>
{
};

class Exec_AND : public Exec_Binary<std::bit_and<Word>>
{
};

class Exec_OR : public Exec_Binary<std::bit_or<Word>>
{
};

class Exec_XOR : public Exec_Binary<std::bit_xor<Word>>
{
};

template <typename Op>
class Exec_Unary : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = Op()(proc->regs_[i.inputs().at(0).reg()]);
    }
};

class Exec_NOT : public Exec_Unary<std::bit_not<Word>>
{
};

class Exec_SET : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = i.inputs().at(0).value();
    }
};

class Exec_LD : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = proc->sram_[i.inputs().at(0).addr()];
    }
};

class Exec_ST : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->sram_[i.outputs().at(0).addr()] = proc->regs_[i.inputs().at(0).reg()];
    }
};

class Exec_JAL : public Executor
{
public:
    uint32_t next_pc(const fabric::VPUInstruction &i, Processor *proc) override
    {
        execute(i, proc);
        return proc->labels_[i.inputs().at(0).label()];
    }
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = proc->pc_;
    }
};

class Exec_RET : public Executor
{
public:
    uint32_t next_pc(const fabric::VPUInstruction &i, Processor *proc) override
    {
        return proc->regs_[i.inputs().at(0).reg()] + 1;
    }
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
    }
};

class Exec_BEQ : public Executor
{
public:
    uint32_t next_pc(const fabric::VPUInstruction &i, Processor *proc) override
    {
        if (proc->regs_[i.inputs().at(0).reg()] == proc->regs_[i.inputs().at(1).reg()])
        {
            return proc->labels_[i.outputs().at(0).label()];
        }
        else
        {
            return proc->pc_ + 1;
        }
    }
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
    }
};

template <typename Op>
class Exec_BinaryPrime : public Executor
{
public:
    void execute(const fabric::VPUInstruction &i, Processor *proc) override
    {
        proc->regs_[i.outputs().at(0).reg()] = Op()(proc->regs_[i.inputs().at(0).reg()], proc->regs_[i.inputs().at(1).reg()]) % proc->p_;
    }
};

class Exec_ADDP : public Exec_BinaryPrime<std::plus<Word>>
{
};

class Exec_SUBP : public Exec_BinaryPrime<std::minus<Word>>
{
};

class Exec_MULP : public Exec_BinaryPrime<std::multiplies<Word>>
{
};

std::unordered_map<fabric::VPUOp, std::unique_ptr<Executor>> Processor::exec_([]()
                                                                              {
   std::unordered_map<fabric::VPUOp, std::unique_ptr<Executor>> m;
   m[fabric::VPU_OP_ADD] = std::make_unique<Exec_ADD>();
   m[fabric::VPU_OP_SUB] = std::make_unique<Exec_SUB>();
   m[fabric::VPU_OP_MUL] = std::make_unique<Exec_MUL>();
   m[fabric::VPU_OP_DIV] = std::make_unique<Exec_DIV>();
   m[fabric::VPU_OP_AND] = std::make_unique<Exec_AND>();
   m[fabric::VPU_OP_OR] = std::make_unique<Exec_OR>();
   m[fabric::VPU_OP_XOR] = std::make_unique<Exec_XOR>();
   m[fabric::VPU_OP_NOT] = std::make_unique<Exec_NOT>();

   m[fabric::VPU_OP_SET] = std::make_unique<Exec_SET>();

   m[fabric::VPU_OP_LD] = std::make_unique<Exec_LD>();
   m[fabric::VPU_OP_ST] = std::make_unique<Exec_ST>();
   m[fabric::VPU_OP_JAL] = std::make_unique<Exec_JAL>();
   m[fabric::VPU_OP_RET] = std::make_unique<Exec_RET>();
   m[fabric::VPU_OP_BEQ] = std::make_unique<Exec_BEQ>();

   m[fabric::VPU_OP_LDP] = std::make_unique<Exec_LDP>();
   m[fabric::VPU_OP_ADDP] = std::make_unique<Exec_ADDP>();
   m[fabric::VPU_OP_SUBP] = std::make_unique<Exec_SUBP>();
   m[fabric::VPU_OP_MULP] = std::make_unique<Exec_MULP>();

   return m; }());

void Processor::run_program(const fabric::VPUProgram &prog)
{
    for (int j = 0; j < prog.instr_size(); ++j)
    {
        const fabric::VPUInstruction &i = prog.instr().at(j);
        if (i.label().size() > 0)
        {
            labels_[i.label()] = j;
        }
    }
    pc_ = 0;
    while (pc_ < prog.instr_size())
    {
        const fabric::VPUInstruction &i = prog.instr().at(pc_);
        pc_ = exec_[i.op()]->next_pc(i, this);
    }
}