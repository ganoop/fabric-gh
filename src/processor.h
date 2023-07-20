

#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <unordered_map>
#include "src/isa.pb.h"

const size_t SRAM_SIZE = 65536;
const size_t NUM_REGS = 32;
typedef int64_t Word;

class Executor;

class Processor
{

public:
    void run_program(const fabric::VPUProgram &prog);

    Word sram_[SRAM_SIZE];
    Word regs_[NUM_REGS];
    Word p_;
    uint32_t pc_;
    std::unordered_map<std::string, uint32_t> labels_;
    static std::unordered_map<fabric::VPUOp, std::unique_ptr<Executor>> exec_;
};

class Executor
{
public:
    virtual uint32_t next_pc(const fabric::VPUInstruction &i, Processor *proc)
    {
        execute(i, proc);
        return proc->pc_ + 1;
    }
    virtual void execute(const fabric::VPUInstruction &i, Processor *proc) = 0;
};

#endif