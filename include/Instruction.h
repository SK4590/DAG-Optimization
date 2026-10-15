#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

enum class InstructionType {
    BINARY,      // res = arg1 op arg2
    UNARY,       // res = op arg1
    COPY,        // res = arg1
    CONSTANT,    // res = 42
    UNKNOWN
};

struct Instruction {
    InstructionType type;
    std::string result;
    std::string arg1;
    std::string op;
    std::string arg2;
    std::string rawLine;

    std::string toString() const {
        if (type == InstructionType::BINARY) {
            return result + " = " + arg1 + " " + op + " " + arg2;
        } else if (type == InstructionType::UNARY) {
            return result + " = " + op + " " + arg1;
        } else if (type == InstructionType::COPY || type == InstructionType::CONSTANT) {
            return result + " = " + arg1;
        }
        return rawLine;
    }
};

#endif // INSTRUCTION_H
