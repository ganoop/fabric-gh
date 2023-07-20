#include "src/tools.h"

std::string inout_to_str(const fabric::InstructionInOut &inout)
{
    std::stringstream stream;
    switch (inout.intype_case())
    {
    case fabric::InstructionInOut::kAddr:
        stream << "0x" << std::hex << inout.addr();
        break;
    case fabric::InstructionInOut::kReg:
        stream << "$R" << inout.reg();
        break;
    case fabric::InstructionInOut::kValue:
        stream << "%" << inout.value();
        break;
    case fabric::InstructionInOut::kLabel:
        stream << "[" << inout.label() << "]";
        break;
    default:
        throw DisassemblyError("Unknown inout type: " + inout.DebugString());
    }
    return stream.str();
}

std::string instr_to_str(const fabric::VPUInstruction &instr)
{
    std::string ret = fabric::VPUOp_Name(instr.op()).substr(7);
    for (const fabric::InstructionInOut &inp : instr.inputs())
    {
        ret += " " + inout_to_str(inp);
    }
    for (const fabric::InstructionInOut &outp : instr.outputs())
    {
        ret += " " + inout_to_str(outp);
    }

    return ret;
}

std::string program_to_str(const fabric::VPUProgram &prog)
{
    std::string ret = "";
    for (const fabric::VPUInstruction &instr : prog.instr())
    {
        ret += instr_to_str(instr) + "\n";
    }
    return ret;
}
