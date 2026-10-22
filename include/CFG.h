#ifndef CFG_H
#define CFG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Instruction.h"
#include "DAGOptimizer.h"

using namespace std;

// ============================================================================
// CFG Edge Types & Structure
// ============================================================================
enum class EdgeType {
    FALLTHROUGH,
    TRUE_BRANCH,
    FALSE_BRANCH,
    UNCONDITIONAL,
    LOOP_BACK
};

struct CFGEdge {
    int from;
    int to;
    EdgeType type;
    string label; // "fallthrough", "True", "False", "unconditional", "Loop"
};

// ============================================================================
// Basic Block Structure
// ============================================================================
struct BasicBlock {
    int id = 0;                          // 0, 1, 2, ...
    string name;                         // "B0", "B1", ...
    vector<Instruction> instructions;
    vector<int> predecessors;            // Predecessor block IDs
    vector<int> successors;              // Successor block IDs

    // Dataflow Analysis Sets for Live Variable Analysis
    unordered_set<string> gen;           // Variables used before def in this block
    unordered_set<string> kill;          // Variables defined in this block
    unordered_set<string> liveIn;        // Variables live at entry of this block
    unordered_set<string> liveOut;       // Variables live at exit of this block

    bool endsWithJump() const {
        if (instructions.empty()) return false;
        return instructions.back().isJump();
    }

    bool endsWithBranch() const {
        if (instructions.empty()) return false;
        return instructions.back().isBranch();
    }

    bool hasLabel(const string& lbl) const {
        for (const auto& inst : instructions) {
            if (inst.type == InstructionType::LABEL && inst.targetLabel == lbl) {
                return true;
            }
        }
        return false;
    }

    vector<string> getLabels() const {
        vector<string> lbls;
        for (const auto& inst : instructions) {
            if (inst.type == InstructionType::LABEL) {
                lbls.push_back(inst.targetLabel);
            }
        }
        return lbls;
    }
};

// ============================================================================
// Control Flow Graph (CFG) Class
// ============================================================================
class CFG {
private:
    vector<BasicBlock> blocks;
    vector<CFGEdge> edges;
    unordered_map<string, int> labelToBlock;

    void identifyLeaders(const vector<Instruction>& instructions, vector<bool>& isLeader);
    void partitionBlocks(const vector<Instruction>& instructions, const vector<bool>& isLeader);
    void buildEdges();
    void computeGenKill();

public:
    CFG();
    void clear();

    // Construct CFG from a sequence of TAC instructions
    void construct(const vector<Instruction>& instructions);

    // Multi-Block Local DAG Optimization: runs DAGOptimizer on each block locally
    void optimizeLocalDAGs();

    // Global Live Variable Analysis: backward dataflow fixed-point iteration
    void runLiveVariableAnalysis();

    // Global Dead Code Elimination: uses OUT[B] to eliminate dead assignments
    int runDeadCodeElimination();

    // Complete Review 3 Optimization Pipeline:
    // 1. Partition into CFG & build edges
    // 2. Multi-block local DAG optimization
    // 3. Iterative Global LVA + Global Dead Code Elimination
    int runFullOptimizationPipeline();

    // Accessors
    const vector<BasicBlock>& getBlocks() const { return blocks; }
    vector<BasicBlock>& getBlocks() { return blocks; }
    const vector<CFGEdge>& getEdges() const { return edges; }
    const BasicBlock* getBlock(int id) const;
    int getBlockCount() const { return (int)blocks.size(); }

    // TAC generation
    vector<Instruction> getAllInstructions() const;
    vector<string> getTACLines() const;
};

#endif // CFG_H
