
#ifndef TOOLS_H
#define TOOLS_H
#include <exception>
#include <iomanip>

#include "src/isa.pb.h"

class DisassemblyError : public std::exception
{
public:
    DisassemblyError(const std::string &msg) : msg_(msg) {}

private:
    virtual const char *what() const throw() { return msg_.c_str(); }
    std::string msg_;
};

std::string inout_to_str(const fabric::InstructionInOut &inout);
std::string instr_to_str(const fabric::VPUInstruction &instr);
std::string program_to_str(const fabric::VPUProgram &prog);

#endif