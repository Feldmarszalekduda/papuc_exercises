#include "RotateRightCyclicInstructionBuilder.hpp"

#include "throw_assert.hpp"
#include <iostream>
#include <regex>
#include <sstream>

namespace ltm
{

RotateRightCyclicInstructionBuilder::RotateRightCyclicInstructionBuilder()
    : InstructionBuilder(std::regex{"\\s+rrc ([a-z])"})
{
}

Instruction RotateRightCyclicInstructionBuilder::buildInstruction(
    const std::smatch& m) const
{
    throw_assert(m.size() == 2, "m contains: " << smatch_print(m));

    const Register reg = registerNameToEnum(m[1]);
    throw_assert(isInternalRegister(reg), "invalid register to rotate");
    return {OperationType::rotateRightCyclic, reg, u8_t{0},
            Register::INVALID};
}
}
