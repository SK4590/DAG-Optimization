#if __has_include("CFG.h")
    #include "CFG.h"
    #include "Utils.h"
    #include "TACParser.h"
#else
    #include "../include/CFG.h"
    #include "../include/Utils.h"
    #include "../include/TACParser.h"
#endif

#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

// ============================================================================
// Helper Utilities for LVA and DCE
// ============================================================================
static bool isTemporary(const string& name) {
    if (name.empty()) return false;
    // Standard compiler temporary prefixes: tmp..., tdead..., t<digit>..., t_<digit>...
    if (name.rfind("tmp", 0) == 0) return true;
    if (name.rfind("tdead", 0) == 0) return true;
    if (name[0] == 't' && name.size() >= 2 && isdigit(name[1])) return true;
    if (name.size() >= 3 && name[0] == 't' && name[1] == '_' && isdigit(name[2])) return true;
    return false;
}

static void getDefAndUses(const Instruction& inst, vector<string>& defs, vector<string>& uses) {
    double dummy;
    if (inst.type == InstructionType::BINARY) {
        if (!inst.result.empty()) defs.push_back(inst.result);
        if (!inst.arg1.empty() && !Utils::isNumber(inst.arg1, dummy)) uses.push_back(inst.arg1);
        if (!inst.arg2.empty() && !Utils::isNumber(inst.arg2, dummy)) uses.push_back(inst.arg2);
    } else if (inst.type == InstructionType::UNARY) {
        if (!inst.result.empty()) defs.push_back(inst.result);
        if (!inst.arg1.empty() && !Utils::isNumber(inst.arg1, dummy)) uses.push_back(inst.arg1);
    } else if (inst.type == InstructionType::COPY) {
        if (!inst.result.empty()) defs.push_back(inst.result);
        if (!inst.arg1.empty() && !Utils::isNumber(inst.arg1, dummy)) uses.push_back(inst.arg1);
    } else if (inst.type == InstructionType::CONSTANT) {
        if (!inst.result.empty()) defs.push_back(inst.result);
    } else if (inst.isBranch()) {
        if (!inst.arg1.empty() && !Utils::isNumber(inst.arg1, dummy)) uses.push_back(inst.arg1);
        if (!inst.arg2.empty() && !Utils::isNumber(inst.arg2, dummy)) uses.push_back(inst.arg2);
    }
}

// ============================================================================
// CFG Implementation
// ============================================================================
CFG::CFG() {
    clear();
}

void CFG::clear() {
    blocks.clear();
    edges.clear();
    labelToBlock.clear();
}

const BasicBlock* CFG::getBlock(int id) const {
    if (id >= 0 && id < (int)blocks.size()) {
        return &blocks[id];
    }
    return nullptr;
}

// ============================================================================
// Standard 3-Rule Leader Identification Algorithm
// (a) First instruction is a leader.
// (b) Target of any conditional or unconditional jump is a leader.
// (c) Any instruction immediately following a jump/branch is a leader.
// ============================================================================
void CFG::identifyLeaders(const vector<Instruction>& instructions, vector<bool>& isLeader) {
    int n = (int)instructions.size();
    isLeader.assign(n, false);
    if (n == 0) return;

    // Index all labels
    unordered_map<string, int> labelToIndex;
    for (int i = 0; i < n; ++i) {
        if (instructions[i].type == InstructionType::LABEL) {
            labelToIndex[instructions[i].targetLabel] = i;
        }
    }

    // Rule (a): The first instruction is a leader
    isLeader[0] = true;

    for (int i = 0; i < n; ++i) {
        // Rule (b): Target of any jump or branch is a leader
        if (instructions[i].isJump() || instructions[i].isBranch()) {
            const string& target = instructions[i].targetLabel;
            if (labelToIndex.count(target)) {
                int targetIdx = labelToIndex[target];
                isLeader[targetIdx] = true;
            }

            // Rule (c): Any instruction immediately following a jump/branch is a leader
            if (i + 1 < n) {
                isLeader[i + 1] = true;
            }
        }

        // Labels themselves inherently denote control-flow entry targets
        if (instructions[i].type == InstructionType::LABEL) {
            isLeader[i] = true;
        }
    }
}

// ============================================================================
// Partition Instruction Stream into BasicBlock Objects
// ============================================================================
void CFG::partitionBlocks(const vector<Instruction>& instructions, const vector<bool>& isLeader) {
    blocks.clear();
    labelToBlock.clear();
    int n = (int)instructions.size();
    if (n == 0) return;

    vector<Instruction> curInsts;
    for (int i = 0; i < n; ++i) {
        if (i > 0 && isLeader[i]) {
            if (!curInsts.empty()) {
                BasicBlock bb;
                bb.id = (int)blocks.size();
                bb.name = "B" + to_string(bb.id);
                bb.instructions = curInsts;
                blocks.push_back(bb);
                curInsts.clear();
            }
        }
        curInsts.push_back(instructions[i]);
    }

    if (!curInsts.empty()) {
        BasicBlock bb;
        bb.id = (int)blocks.size();
        bb.name = "B" + to_string(bb.id);
        bb.instructions = curInsts;
        blocks.push_back(bb);
    }

    // Map labels to blocks
    for (const auto& bb : blocks) {
        for (const auto& inst : bb.instructions) {
            if (inst.type == InstructionType::LABEL) {
                labelToBlock[inst.targetLabel] = bb.id;
            }
        }
    }
}

// ============================================================================
// Construct Directed Control Edges Between Blocks
// ============================================================================
void CFG::buildEdges() {
    edges.clear();
    for (auto& b : blocks) {
        b.predecessors.clear();
        b.successors.clear();
    }

    int numBlocks = (int)blocks.size();
    for (int i = 0; i < numBlocks; ++i) {
        const auto& insts = blocks[i].instructions;
        if (insts.empty()) {
            if (i + 1 < numBlocks) {
                edges.push_back({i, i + 1, EdgeType::FALLTHROUGH, "fallthrough"});
                blocks[i].successors.push_back(i + 1);
                blocks[i + 1].predecessors.push_back(i);
            }
            continue;
        }

        const Instruction& last = insts.back();

        if (last.type == InstructionType::GOTO) {
            // Unconditional jump to target block
            if (labelToBlock.count(last.targetLabel)) {
                int targetId = labelToBlock[last.targetLabel];
                EdgeType eType = (targetId <= i) ? EdgeType::LOOP_BACK : EdgeType::UNCONDITIONAL;
                string eLabel = (targetId <= i) ? "Loop" : "unconditional";
                edges.push_back({i, targetId, eType, eLabel});
                blocks[i].successors.push_back(targetId);
                blocks[targetId].predecessors.push_back(i);
            }
        } else if (last.isBranch()) {
            // 1. Conditional Branch Target (True/False)
            if (labelToBlock.count(last.targetLabel)) {
                int targetId = labelToBlock[last.targetLabel];
                EdgeType bType = (targetId <= i) ? EdgeType::LOOP_BACK :
                                 (last.type == InstructionType::IF_GOTO ? EdgeType::TRUE_BRANCH : EdgeType::FALSE_BRANCH);
                string bLabel = (last.type == InstructionType::IF_GOTO) ? "True" : "False";
                if (targetId <= i) bLabel += " (Loop)";
                edges.push_back({i, targetId, bType, bLabel});
                blocks[i].successors.push_back(targetId);
                blocks[targetId].predecessors.push_back(i);
            }

            // 2. Fallthrough Edge to next block
            if (i + 1 < numBlocks) {
                EdgeType ftType = (last.type == InstructionType::IF_GOTO) ? EdgeType::FALSE_BRANCH : EdgeType::TRUE_BRANCH;
                string ftLabel = (last.type == InstructionType::IF_GOTO) ? "False" : "True";
                edges.push_back({i, i + 1, ftType, ftLabel});
                blocks[i].successors.push_back(i + 1);
                blocks[i + 1].predecessors.push_back(i);
            }
        } else {
            // Normal sequential fallthrough to next block
            if (i + 1 < numBlocks) {
                edges.push_back({i, i + 1, EdgeType::FALLTHROUGH, "fallthrough"});
                blocks[i].successors.push_back(i + 1);
                blocks[i + 1].predecessors.push_back(i);
            }
        }
    }
}

// ============================================================================
// Full Construction Method
// ============================================================================
void CFG::construct(const vector<Instruction>& instructions) {
    clear();
    vector<bool> isLeader;
    identifyLeaders(instructions, isLeader);
    partitionBlocks(instructions, isLeader);
    buildEdges();
    runLiveVariableAnalysis();
}

// ============================================================================
// Multi-Block DAG Optimization Pass
// Runs local DAG optimization on every basic block, preserving control flow.
// ============================================================================
void CFG::optimizeLocalDAGs() {
    for (auto& b : blocks) {
        DAGOptimizer blockOpt;
        blockOpt.processBlock(b.instructions);
        b.instructions = blockOpt.generateOptimizedInstructions();
    }
    // Re-index labels in case any changed
    labelToBlock.clear();
    for (const auto& bb : blocks) {
        for (const auto& inst : bb.instructions) {
            if (inst.type == InstructionType::LABEL) {
                labelToBlock[inst.targetLabel] = bb.id;
            }
        }
    }
    buildEdges();
    runLiveVariableAnalysis();
}

// ============================================================================
// GEN and KILL computation for Basic Blocks
// GEN[B]: variables used in B prior to any assignment in B
// KILL[B]: variables assigned a value in B
// ============================================================================
void CFG::computeGenKill() {
    for (auto& b : blocks) {
        b.gen.clear();
        b.kill.clear();
        for (const auto& inst : b.instructions) {
            vector<string> defs, uses;
            getDefAndUses(inst, defs, uses);

            // Variable used before definition in this block
            for (const string& u : uses) {
                if (b.kill.find(u) == b.kill.end()) {
                    b.gen.insert(u);
                }
            }

            // Variable defined in this block
            for (const string& d : defs) {
                b.kill.insert(d);
            }
        }
    }
}

// ============================================================================
// Global Live Variable Analysis (LVA)
// Backward dataflow analysis computing IN[B] and OUT[B] until fixed-point:
// OUT[B] = Union_{S in Succ(B)} IN[S]
// IN[B]  = GEN[B] U (OUT[B] \ KILL[B])
// ============================================================================
void CFG::runLiveVariableAnalysis() {
    computeGenKill();

    for (auto& b : blocks) {
        b.liveIn.clear();
        b.liveOut.clear();
    }

    // Collect global/user variables that should remain live at exit blocks
    unordered_set<string> globalVars;
    for (const auto& b : blocks) {
        for (const auto& inst : b.instructions) {
            if (!inst.result.empty() && !isTemporary(inst.result)) {
                globalVars.insert(inst.result);
            }
        }
    }

    // Initialize exit blocks (blocks with no successors) with global live variables
    for (auto& b : blocks) {
        if (b.successors.empty()) {
            b.liveOut = globalVars;
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // Backward sweep across blocks
        for (int i = (int)blocks.size() - 1; i >= 0; --i) {
            BasicBlock& B = blocks[i];

            // OUT[B] = Union_{S in succ(B)} IN[S]
            if (!B.successors.empty()) {
                unordered_set<string> newOut;
                for (int succId : B.successors) {
                    if (succId >= 0 && succId < (int)blocks.size()) {
                        for (const string& v : blocks[succId].liveIn) {
                            newOut.insert(v);
                        }
                    }
                }
                B.liveOut = newOut;
            }

            // IN[B] = GEN[B] U (OUT[B] - KILL[B])
            unordered_set<string> newIn = B.gen;
            for (const string& v : B.liveOut) {
                if (B.kill.find(v) == B.kill.end()) {
                    newIn.insert(v);
                }
            }

            if (newIn != B.liveIn) {
                B.liveIn = newIn;
                changed = true;
            }
        }
    }
}

// ============================================================================
// Global Dead Code Elimination (DCE)
// Uses OUT[B] to eliminate dead temporary assignments (e.g., t2 = t1 where t2
// is never read again downstream across the CFG).
// ============================================================================
int CFG::runDeadCodeElimination() {
    int totalEliminated = 0;
    bool globalChanged = true;

    while (globalChanged) {
        globalChanged = false;
        runLiveVariableAnalysis();

        for (auto& B : blocks) {
            unordered_set<string> live = B.liveOut;
            vector<Instruction> survivingInsts;

            // Traverse instructions backwards from last to first
            for (int i = (int)B.instructions.size() - 1; i >= 0; --i) {
                const auto& inst = B.instructions[i];

                // Labels, jumps, and conditional branches cannot be eliminated by assignment DCE
                if (inst.isControlFlow()) {
                    survivingInsts.push_back(inst);
                    vector<string> defs, uses;
                    getDefAndUses(inst, defs, uses);
                    for (const auto& u : uses) {
                        live.insert(u);
                    }
                    continue;
                }

                // Check if instruction defines a variable
                if (!inst.result.empty()) {
                    // Dead temporary assignment detection
                    if (isTemporary(inst.result) && live.find(inst.result) == live.end()) {
                        totalEliminated++;
                        globalChanged = true;
                        continue; // Eliminate dead instruction
                    }
                }

                // Instruction survives
                survivingInsts.push_back(inst);

                // Update live set for preceding statements: remove defs, add uses
                vector<string> defs, uses;
                getDefAndUses(inst, defs, uses);
                for (const auto& d : defs) {
                    live.erase(d);
                }
                for (const auto& u : uses) {
                    live.insert(u);
                }
            }

            // Reverse surviving instructions back to forward sequential order
            reverse(survivingInsts.begin(), survivingInsts.end());
            B.instructions = survivingInsts;
        }
    }

    // Refresh dataflow sets post-elimination
    runLiveVariableAnalysis();
    return totalEliminated;
}

// ============================================================================
// Full Multi-Block Optimization Pipeline
// ============================================================================
int CFG::runFullOptimizationPipeline() {
    optimizeLocalDAGs();
    int deadEliminated = runDeadCodeElimination();
    return deadEliminated;
}

// ============================================================================
// Instruction and TAC Emission Helpers
// ============================================================================
vector<Instruction> CFG::getAllInstructions() const {
    vector<Instruction> all;
    for (const auto& b : blocks) {
        for (const auto& inst : b.instructions) {
            all.push_back(inst);
        }
    }
    return all;
}

vector<string> CFG::getTACLines() const {
    vector<string> lines;
    for (const auto& b : blocks) {
        for (const auto& inst : b.instructions) {
            lines.push_back(inst.toString());
        }
    }
    return lines;
}
