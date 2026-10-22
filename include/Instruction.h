#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

using namespace std;

enum class InstructionType {
    BINARY,         // res = arg1 op arg2
    UNARY,          // res = op arg1
    COPY,           // res = arg1
    CONSTANT,       // res = 42
    LABEL,          // L1:
    GOTO,           // goto L1
    IF_GOTO,        // if x < y goto L1 or if x goto L1
    IF_FALSE_GOTO,  // ifFalse a == b goto L2 or ifFalse a goto L2
    UNKNOWN
};

struct Instruction {
    InstructionType type = InstructionType::UNKNOWN;
    string result;
    string arg1;
    string op;
    string arg2;
    string targetLabel; // Target label for jumps, branches, or label definitions
    string rawLine;

    bool isJump() const {
        return type == InstructionType::GOTO;
    }

    bool isBranch() const {
        return type == InstructionType::IF_GOTO || type == InstructionType::IF_FALSE_GOTO;
    }

    bool isLabel() const {
        return type == InstructionType::LABEL;
    }

    bool isControlFlow() const {
        return isJump() || isBranch() || isLabel();
    }

    string toString() const {
        if (type == InstructionType::BINARY) {
            return result + " = " + arg1 + " " + op + " " + arg2;
        } else if (type == InstructionType::UNARY) {
            return result + " = " + op + " " + arg1;
        } else if (type == InstructionType::COPY || type == InstructionType::CONSTANT) {
            return result + " = " + arg1;
        } else if (type == InstructionType::LABEL) {
            return targetLabel + ":";
        } else if (type == InstructionType::GOTO) {
            return "goto " + targetLabel;
        } else if (type == InstructionType::IF_GOTO) {
            if (!op.empty() && !arg2.empty()) {
                return "if " + arg1 + " " + op + " " + arg2 + " goto " + targetLabel;
            } else {
                return "if " + arg1 + " goto " + targetLabel;
            }
        } else if (type == InstructionType::IF_FALSE_GOTO) {
            if (!op.empty() && !arg2.empty()) {
                return "ifFalse " + arg1 + " " + op + " " + arg2 + " goto " + targetLabel;
            } else {
                return "ifFalse " + arg1 + " goto " + targetLabel;
            }
        }
        return rawLine;
    }
};

#endif // INSTRUCTION_H
