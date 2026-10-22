#if __has_include("CodeGen.h")
    #include "CodeGen.h"
    #include "Utils.h"
#elif __has_include("include/CodeGen.h")
    #include "include/CodeGen.h"
    #include "include/Utils.h"
#else
    #include "../include/CodeGen.h"
    #include "../include/Utils.h"
#endif

#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

CodeGen::CodeGen() {
    reset();
}

void CodeGen::reset() {
    allocator.reset();
    assemblyCode.clear();
    metrics = CodeGenMetrics();
}

void CodeGen::setRegisterPoolCapacity(int capacity) {
    allocator.setPoolCapacity(capacity);
}

void CodeGen::emitConstant(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    string destReg = allocator.getRegForWrite(inst.result, assemblyCode);
    double dummy;
    long long intVal = 0;
    if (Utils::isNumber(inst.arg1, dummy)) {
        intVal = (long long)dummy;
    }
    assemblyCode.push_back("    li " + destReg + ", " + to_string(intVal));
    metrics.immediateLoads++;
    metrics.totalAssemblyLines++;
}

void CodeGen::emitCopy(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    double dummy;
    if (Utils::isNumber(inst.arg1, dummy)) {
        emitConstant(inst);
        return;
    }

    string srcReg = allocator.getRegForRead(inst.arg1, assemblyCode);
    string destReg = allocator.getRegForWrite(inst.result, assemblyCode);
    if (destReg != srcReg) {
        assemblyCode.push_back("    move " + destReg + ", " + srcReg);
        metrics.moveInstructions++;
        metrics.totalAssemblyLines++;
    }
}

void CodeGen::emitUnary(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    string srcReg = allocator.getRegForRead(inst.arg1, assemblyCode);
    string destReg = allocator.getRegForWrite(inst.result, assemblyCode);

    if (inst.op == "-") {
        assemblyCode.push_back("    neg " + destReg + ", " + srcReg);
    } else if (inst.op == "!" || inst.op == "not") {
        assemblyCode.push_back("    seq " + destReg + ", " + srcReg + ", $zero");
    } else if (inst.op == "~") {
        assemblyCode.push_back("    not " + destReg + ", " + srcReg);
    } else {
        assemblyCode.push_back("    move " + destReg + ", " + srcReg);
    }
    metrics.arithmeticInstructions++;
    metrics.totalAssemblyLines++;
}

void CodeGen::emitBinary(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    string r1 = allocator.getRegForRead(inst.arg1, assemblyCode);
    string r2 = allocator.getRegForRead(inst.arg2, assemblyCode);
    string rd = allocator.getRegForWrite(inst.result, assemblyCode);

    string asmOp = "";
    if (inst.op == "+") asmOp = "add";
    else if (inst.op == "-") asmOp = "sub";
    else if (inst.op == "*") asmOp = "mul";
    else if (inst.op == "/") asmOp = "div";
    else if (inst.op == "%") asmOp = "rem";
    else if (inst.op == "&") asmOp = "and";
    else if (inst.op == "|") asmOp = "or";
    else if (inst.op == "^") asmOp = "xor";
    else if (inst.op == "<") asmOp = "slt";
    else if (inst.op == ">") asmOp = "sgt";
    else if (inst.op == "<=") asmOp = "sle";
    else if (inst.op == ">=") asmOp = "sge";
    else if (inst.op == "==") asmOp = "seq";
    else if (inst.op == "!=") asmOp = "sne";
    else asmOp = "add";

    assemblyCode.push_back("    " + asmOp + " " + rd + ", " + r1 + ", " + r2);
    metrics.arithmeticInstructions++;
    metrics.totalAssemblyLines++;
}

void CodeGen::emitLabel(const Instruction& inst) {
    // Before a label, flush dirty registers to stack and clear bindings
    allocator.spillAll(assemblyCode);
    allocator.clearBindings();
    assemblyCode.push_back(inst.targetLabel + ":");
}

void CodeGen::emitJump(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    // Before jump, spill dirty registers to preserve state
    allocator.spillAll(assemblyCode);
    assemblyCode.push_back("    j " + inst.targetLabel);
    metrics.branchInstructions++;
    metrics.totalAssemblyLines++;
}

void CodeGen::emitBranch(const Instruction& inst) {
    assemblyCode.push_back("    # TAC: " + inst.toString());
    bool isIfFalse = (inst.type == InstructionType::IF_FALSE_GOTO);

    if (inst.op.empty() || inst.arg2.empty()) {
        // Unary condition: if x goto L or ifFalse x goto L
        string r1 = allocator.getRegForRead(inst.arg1, assemblyCode);
        allocator.spillAll(assemblyCode);
        string branchOp = isIfFalse ? "beq" : "bne";
        assemblyCode.push_back("    " + branchOp + " " + r1 + ", $zero, " + inst.targetLabel);
    } else {
        // Binary condition: if x op y goto L or ifFalse x op y goto L
        string r1 = allocator.getRegForRead(inst.arg1, assemblyCode);
        string r2 = allocator.getRegForRead(inst.arg2, assemblyCode);
        allocator.spillAll(assemblyCode);

        string branchOp = "";
        if (!isIfFalse) {
            if (inst.op == "<") branchOp = "blt";
            else if (inst.op == "<=") branchOp = "ble";
            else if (inst.op == ">") branchOp = "bgt";
            else if (inst.op == ">=") branchOp = "bge";
            else if (inst.op == "==") branchOp = "beq";
            else if (inst.op == "!=") branchOp = "bne";
            else branchOp = "bne";
        } else {
            // Inverted for ifFalse
            if (inst.op == "<") branchOp = "bge";
            else if (inst.op == "<=") branchOp = "bgt";
            else if (inst.op == ">") branchOp = "ble";
            else if (inst.op == ">=") branchOp = "blt";
            else if (inst.op == "==") branchOp = "bne";
            else if (inst.op == "!=") branchOp = "beq";
            else branchOp = "beq";
        }
        assemblyCode.push_back("    " + branchOp + " " + r1 + ", " + r2 + ", " + inst.targetLabel);
    }

    metrics.branchInstructions++;
    metrics.totalAssemblyLines++;
}

void CodeGen::emitInstruction(const Instruction& inst) {
    switch (inst.type) {
        case InstructionType::CONSTANT:
            emitConstant(inst);
            break;
        case InstructionType::COPY:
            emitCopy(inst);
            break;
        case InstructionType::UNARY:
            emitUnary(inst);
            break;
        case InstructionType::BINARY:
            emitBinary(inst);
            break;
        case InstructionType::LABEL:
            emitLabel(inst);
            break;
        case InstructionType::GOTO:
            emitJump(inst);
            break;
        case InstructionType::IF_GOTO:
        case InstructionType::IF_FALSE_GOTO:
            emitBranch(inst);
            break;
        default:
            break;
    }
}

string CodeGen::generateAssembly(const vector<Instruction>& instructions) {
    reset();

    vector<string> bodyInstructions;
    // Swap assemblyCode pointer temporarily to collect body instructions
    assemblyCode.clear();

    for (const auto& inst : instructions) {
        emitInstruction(inst);
    }

    // Flush any remaining dirty variables to stack before exit
    allocator.spillAll(assemblyCode);

    bodyInstructions = assemblyCode;
    assemblyCode.clear();

    int frameSize = allocator.getStackFrameSize();
    int raOffset = frameSize - 4;
    int fpOffset = frameSize - 8;

    // Header and Data Segment
    assemblyCode.push_back("# ============================================================================");
    assemblyCode.push_back("# Generated Target Machine Assembly (MIPS Architecture)");
    assemblyCode.push_back("# Compiler Backend: Register Allocator & Target Code Generator");
    assemblyCode.push_back("# Frame Size: " + to_string(frameSize) + " bytes");
    assemblyCode.push_back("# ============================================================================");
    assemblyCode.push_back("");
    assemblyCode.push_back(".data");
    assemblyCode.push_back("    _newline: .asciiz \"\\n\"");
    assemblyCode.push_back("");
    assemblyCode.push_back(".text");
    assemblyCode.push_back(".globl main");
    assemblyCode.push_back("");
    assemblyCode.push_back("main:");
    assemblyCode.push_back("    # --- Function Prologue ---");
    assemblyCode.push_back("    addi $sp, $sp, -" + to_string(frameSize));
    assemblyCode.push_back("    sw $ra, " + to_string(raOffset) + "($sp)");
    assemblyCode.push_back("    sw $fp, " + to_string(fpOffset) + "($sp)");
    assemblyCode.push_back("    move $fp, $sp");
    assemblyCode.push_back("");

    // Body Instructions
    for (const auto& line : bodyInstructions) {
        assemblyCode.push_back(line);
    }

    // Epilogue
    assemblyCode.push_back("");
    assemblyCode.push_back("    # --- Function Epilogue & Exit ---");
    assemblyCode.push_back("    move $sp, $fp");
    assemblyCode.push_back("    lw $fp, " + to_string(fpOffset) + "($sp)");
    assemblyCode.push_back("    lw $ra, " + to_string(raOffset) + "($sp)");
    assemblyCode.push_back("    addi $sp, $sp, " + to_string(frameSize));
    assemblyCode.push_back("    li $v0, 10             # syscall 10: program exit");
    assemblyCode.push_back("    syscall");

    // Update metrics from allocator
    const auto& aStats = allocator.getStats();
    metrics.memoryInstructions = aStats.memoryLoads + aStats.memoryStores;
    metrics.totalSpills = aStats.registerSpills;
    metrics.totalAssemblyLines = (int)assemblyCode.size();

    // Form string
    stringstream ss;
    for (const auto& line : assemblyCode) {
        ss << line << "\n";
    }
    return ss.str();
}

bool CodeGen::writeAssemblyToFile(const vector<Instruction>& instructions, const string& filepath) {
    string asmContent = generateAssembly(instructions);
    ofstream outFile(filepath);
    if (!outFile.is_open()) {
        cerr << "[ERROR] Could not open file for writing assembly: " << filepath << "\n";
        return false;
    }
    outFile << asmContent;
    outFile.close();
    return true;
}
