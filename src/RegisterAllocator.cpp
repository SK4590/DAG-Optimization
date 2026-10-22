#if __has_include("RegisterAllocator.h")
    #include "RegisterAllocator.h"
    #include "Utils.h"
#else
    #include "../include/RegisterAllocator.h"
    #include "../include/Utils.h"
#endif

#include <algorithm>
#include <iomanip>
#include <climits>

using namespace std;

RegisterAllocator::RegisterAllocator() {
    reset();
}

void RegisterAllocator::reset() {
    registers.clear();
    regNameToIndex.clear();
    varTable.clear();
    timer = 0;
    nextStackOffset = 0;
    stats = AllocatorStats();

    // Default MIPS register pool: 10 temporaries ($t0-$t9) and 8 saved registers ($s0-$s7)
    vector<string> defaultPool = {
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7"
    };
    initPool(defaultPool);
}

void RegisterAllocator::initPool(const vector<string>& pool) {
    registers.clear();
    regNameToIndex.clear();
    for (size_t i = 0; i < pool.size(); ++i) {
        RegDescriptor desc;
        desc.name = pool[i];
        desc.currentVar = "";
        desc.isFree = true;
        desc.isDirty = false;
        desc.lastUsed = 0;
        registers.push_back(desc);
        regNameToIndex[pool[i]] = (int)i;
    }
}

void RegisterAllocator::setPoolCapacity(int capacity) {
    if (capacity <= 0 || capacity > (int)registers.size()) return;
    registers.resize(capacity);
    regNameToIndex.clear();
    for (size_t i = 0; i < registers.size(); ++i) {
        regNameToIndex[registers[i].name] = (int)i;
    }
}

int RegisterAllocator::findFreeRegister() {
    for (size_t i = 0; i < registers.size(); ++i) {
        if (registers[i].isFree) {
            return (int)i;
        }
    }
    return -1;
}

int RegisterAllocator::selectVictimRegister() {
    int oldestTime = INT_MAX;
    int victimIdx = 0;
    for (size_t i = 0; i < registers.size(); ++i) {
        if (registers[i].lastUsed < oldestTime) {
            oldestTime = registers[i].lastUsed;
            victimIdx = (int)i;
        }
    }
    return victimIdx;
}

void RegisterAllocator::spillRegister(int regIdx, vector<string>& emittedAsm) {
    if (regIdx < 0 || regIdx >= (int)registers.size()) return;
    auto& reg = registers[regIdx];
    if (reg.isFree) return;

    string var = reg.currentVar;
    if (!var.empty()) {
        if (reg.isDirty) {
            auto& loc = varTable[var];
            if (!loc.inMemory || loc.stackOffset < 0) {
                loc.varName = var;
                loc.stackOffset = nextStackOffset;
                nextStackOffset += 4;
                loc.inMemory = true;
            }
            emittedAsm.push_back("    sw " + reg.name + ", " + 
                                 to_string(loc.stackOffset) + "($sp)    # spill " + var + " to stack");
            stats.memoryStores++;
            stats.registerSpills++;
        }
        varTable[var].inRegister = "";
    }

    reg.currentVar = "";
    reg.isFree = true;
    reg.isDirty = false;
}

void RegisterAllocator::spillAll(vector<string>& emittedAsm) {
    for (size_t i = 0; i < registers.size(); ++i) {
        if (!registers[i].isFree && registers[i].isDirty) {
            spillRegister((int)i, emittedAsm);
        }
    }
}

void RegisterAllocator::clearBindings() {
    for (auto& reg : registers) {
        reg.currentVar = "";
        reg.isFree = true;
        reg.isDirty = false;
    }
    for (auto& kv : varTable) {
        kv.second.inRegister = "";
    }
}

string RegisterAllocator::getRegForRead(const string& varOrConst, vector<string>& emittedAsm) {
    double dummy;
    timer++;

    // 1. If operand is a literal constant number
    if (Utils::isNumber(varOrConst, dummy)) {
        int regIdx = findFreeRegister();
        if (regIdx == -1) {
            regIdx = selectVictimRegister();
            spillRegister(regIdx, emittedAsm);
        }
        auto& reg = registers[regIdx];
        reg.isFree = false;
        reg.currentVar = ""; // Ephemeral constant
        reg.isDirty = false;
        reg.lastUsed = timer;

        long long intVal = (long long)dummy;
        emittedAsm.push_back("    li " + reg.name + ", " + to_string(intVal) + "         # load immediate " + varOrConst);
        stats.registerAllocations++;
        return reg.name;
    }

    // 2. If operand is a variable already residing in a register
    if (varTable.count(varOrConst) && !varTable[varOrConst].inRegister.empty()) {
        string regName = varTable[varOrConst].inRegister;
        if (regNameToIndex.count(regName)) {
            int idx = regNameToIndex[regName];
            registers[idx].lastUsed = timer;
            return regName;
        }
    }

    // 3. Variable needs to be loaded into a register
    int regIdx = findFreeRegister();
    if (regIdx == -1) {
        regIdx = selectVictimRegister();
        spillRegister(regIdx, emittedAsm);
    }

    auto& reg = registers[regIdx];
    reg.isFree = false;
    reg.currentVar = varOrConst;
    reg.isDirty = false;
    reg.lastUsed = timer;

    auto& loc = varTable[varOrConst];
    loc.varName = varOrConst;
    loc.inRegister = reg.name;

    if (loc.inMemory && loc.stackOffset >= 0) {
        emittedAsm.push_back("    lw " + reg.name + ", " + to_string(loc.stackOffset) + "($sp)    # restore " + varOrConst + " from stack");
        stats.memoryLoads++;
    } else {
        // First encounter of variable without explicit store; assign stack offset
        loc.stackOffset = nextStackOffset;
        nextStackOffset += 4;
        loc.inMemory = false;
    }

    stats.registerAllocations++;
    int activeRegs = 0;
    for (const auto& r : registers) if (!r.isFree) activeRegs++;
    stats.maxRegistersUsed = max(stats.maxRegistersUsed, activeRegs);

    return reg.name;
}

string RegisterAllocator::getRegForWrite(const string& varName, vector<string>& emittedAsm) {
    timer++;

    // If variable already holds a register, mark it dirty and return
    if (varTable.count(varName) && !varTable[varName].inRegister.empty()) {
        string regName = varTable[varName].inRegister;
        if (regNameToIndex.count(regName)) {
            int idx = regNameToIndex[regName];
            registers[idx].isDirty = true;
            registers[idx].lastUsed = timer;
            return regName;
        }
    }

    // Allocate a new register for destination
    int regIdx = findFreeRegister();
    if (regIdx == -1) {
        regIdx = selectVictimRegister();
        spillRegister(regIdx, emittedAsm);
    }

    auto& reg = registers[regIdx];
    reg.isFree = false;
    reg.currentVar = varName;
    reg.isDirty = true;
    reg.lastUsed = timer;

    auto& loc = varTable[varName];
    loc.varName = varName;
    loc.inRegister = reg.name;
    if (loc.stackOffset < 0) {
        loc.stackOffset = nextStackOffset;
        nextStackOffset += 4;
    }

    stats.registerAllocations++;
    int activeRegs = 0;
    for (const auto& r : registers) if (!r.isFree) activeRegs++;
    stats.maxRegistersUsed = max(stats.maxRegistersUsed, activeRegs);

    return reg.name;
}

int RegisterAllocator::getStackFrameSize() const {
    // Standard MIPS ABI: align stack frame size to 16 bytes with minimum frame of 32 bytes
    int rawSize = nextStackOffset + 8; // Extra space for $ra and $fp
    int aligned = ((rawSize + 15) / 16) * 16;
    return max(32, aligned);
}

bool RegisterAllocator::hasVariable(const string& varName) const {
    return varTable.count(varName) > 0;
}

const VarLocation* RegisterAllocator::getVarLocation(const string& varName) const {
    auto it = varTable.find(varName);
    if (it != varTable.end()) {
        return &(it->second);
    }
    return nullptr;
}

void RegisterAllocator::printDescriptors() const {
    cout << "\n--- Register Descriptors ---\n";
    for (const auto& r : registers) {
        cout << "  " << left << setw(6) << r.name 
             << " | " << (r.isFree ? "[FREE]" : "Var: " + r.currentVar)
             << (r.isDirty ? " (DIRTY)" : "") << "\n";
    }
    cout << "--- Address Descriptors (" << varTable.size() << " variables) ---\n";
    for (const auto& kv : varTable) {
        cout << "  " << left << setw(12) << kv.first 
             << " | Reg: " << (kv.second.inRegister.empty() ? "[None]" : kv.second.inRegister)
             << " | Stack: " << kv.second.stackOffset << "($sp)\n";
    }
    cout << "----------------------------\n\n";
}
