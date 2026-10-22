#ifndef CODEGEN_H
#define CODEGEN_H

#include <string>
#include <vector>
#include "Instruction.h"
#include "RegisterAllocator.h"

using namespace std;

// ============================================================================
// Code Generator Metrics
// ============================================================================
struct CodeGenMetrics {
    int totalAssemblyLines = 0;
    int arithmeticInstructions = 0;
    int memoryInstructions = 0;
    int branchInstructions = 0;
    int moveInstructions = 0;
    int immediateLoads = 0;
    int totalSpills = 0;
};

// ============================================================================
// Target Machine Assembly Code Generator (MIPS Architecture)
// Translates 3-Address Code into standard MIPS assembly instructions.
// ============================================================================
class CodeGen {
private:
    RegisterAllocator allocator;
    vector<string> assemblyCode;
    CodeGenMetrics metrics;

    void emitInstruction(const Instruction& inst);
    void emitBinary(const Instruction& inst);
    void emitUnary(const Instruction& inst);
    void emitCopy(const Instruction& inst);
    void emitConstant(const Instruction& inst);
    void emitBranch(const Instruction& inst);
    void emitJump(const Instruction& inst);
    void emitLabel(const Instruction& inst);

public:
    CodeGen();
    void reset();

    // Set capacity of register pool if needed for testing/demonstration
    void setRegisterPoolCapacity(int capacity);

    // Generate complete assembly program from instructions
    string generateAssembly(const vector<Instruction>& instructions);

    // Write generated assembly to file (.s / .asm)
    bool writeAssemblyToFile(const vector<Instruction>& instructions, const string& filepath);

    // Accessors
    const vector<string>& getAssemblyLines() const { return assemblyCode; }
    const CodeGenMetrics& getMetrics() const { return metrics; }
    const RegisterAllocator& getAllocator() const { return allocator; }
};

#endif // CODEGEN_H
