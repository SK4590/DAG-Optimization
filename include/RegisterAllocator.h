#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;

// ============================================================================
// Register Descriptor
// Tracks the state of a single target machine register
// ============================================================================
struct RegDescriptor {
    string name;              // e.g. "$t0", "$t1", "$s0"
    string currentVar;        // Variable currently residing in register (or empty)
    bool isFree = true;       // True if no variable is mapped
    bool isDirty = false;     // True if value was modified and not yet saved to memory
    int lastUsed = 0;         // Timestamp for Least-Recently-Used (LRU) spilling
};

// ============================================================================
// Address Descriptor
// Tracks the location of a variable (register and/or stack memory)
// ============================================================================
struct VarLocation {
    string varName;
    string inRegister;        // Register name if currently resident (or empty)
    int stackOffset = -1;     // Offset relative to $sp (e.g. 0, 4, 8, ...)
    bool inMemory = false;    // True if a valid copy exists in stack memory
};

// ============================================================================
// Register Allocator Statistics
// ============================================================================
struct AllocatorStats {
    int registerAllocations = 0;
    int registerSpills = 0;
    int memoryLoads = 0;
    int memoryStores = 0;
    int maxRegistersUsed = 0;
};

// ============================================================================
// Register Allocator Class
// Implements register allocation with LRU spilling for MIPS architecture
// ============================================================================
class RegisterAllocator {
private:
    vector<RegDescriptor> registers;
    unordered_map<string, int> regNameToIndex;
    unordered_map<string, VarLocation> varTable;
    int timer = 0;
    int nextStackOffset = 0;  // Grows by 4 bytes per spilled variable
    AllocatorStats stats;

    int findFreeRegister();
    int selectVictimRegister(); // LRU policy

public:
    RegisterAllocator();
    void reset();

    // Configure register pool (default: MIPS $t0-$t9, $s0-$s7)
    void initPool(const vector<string>& pool);
    void setPoolCapacity(int capacity);

    // Get register for reading an operand (var or constant)
    string getRegForRead(const string& varOrConst, vector<string>& emittedAsm);

    // Get register for writing a destination result variable
    string getRegForWrite(const string& varName, vector<string>& emittedAsm);

    // Explicitly spill a single register to stack memory
    void spillRegister(int regIdx, vector<string>& emittedAsm);

    // Spill all dirty registers to stack (used at block boundaries / jumps)
    void spillAll(vector<string>& emittedAsm);

    // Invalidate register bindings (used after unconditional jumps or labels)
    void clearBindings();

    // Memory and stack queries
    int getStackFrameSize() const;
    bool hasVariable(const string& varName) const;
    const VarLocation* getVarLocation(const string& varName) const;
    const AllocatorStats& getStats() const { return stats; }

    // Debug & Inspection
    void printDescriptors() const;
};

#endif // REGISTER_ALLOCATOR_H
