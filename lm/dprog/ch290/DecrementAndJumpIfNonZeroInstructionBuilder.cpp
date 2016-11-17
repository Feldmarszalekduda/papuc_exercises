#include "DecrementAndJumpIfNonZeroInstructionBuilder.hpp"

#include "throw_assert.hpp"
#include <iostream>
#include <regex>
#include <sstream>

namespace ltm
{

DecrementAndJumpIfNonZeroInstructionBuilder::
    DecrementAndJumpIfNonZeroInstructionBuilder()
        : InstructionBuilder(std::regex{"\\s+djnz ([a-z]) ([0-9]+)"})
{
}

Instruction DecrementAndJumpIfNonZeroInstructionBuilder::buildInstruction(
    const std::smatch& m) const
{
    throw_assert(m.size() == 3, "m contains: " << smatch_print(m));

    const Register reg = registerNameToEnum(m[1]);
    throw_assert(isInternalRegister(reg), "invalid register to rotate");

    const u8_t label_id = std::stoi(m[2]);
    return {OperationType::decrementAndJump, reg, label_id,
            Register::INVALID};
}
}