#ifndef DAGOPTIMIZER_H
#define DAGOPTIMIZER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Instruction.h"
#include "DAGNode.h"

using namespace std;

class DAGOptimizer {
private:
    vector<DAGNode> nodes;
    unordered_map<string, int> valueNumbering; // Signature -> Node ID
    unordered_map<string, int> varToNode;      // Variable Name -> Node ID
    unordered_map<string, int> constToNode;    // Formatted Const String -> Node ID
    vector<string> optimizationLog;
    vector<ProcessedStep> steps;
    OptimizationStats stats;

    string makeSignature(const string& op, int leftId, int rightId);
    string makeUnarySignature(const string& op, int childId);
    void detachVariable(const string& varName);
    void attachVariable(const string& varName, int targetNodeId);
    string getActiveVarForNode(int nodeId, const string& preferredName = "") const;
    int getOrCreateLeaf(const string& name);
    int getOrCreateConstantNode(double val);
    bool evaluateConstant(const string& op, double v1, double v2, double& result);
    int checkAlgebraicIdentity(const string& op, int leftId, int rightId, string& reason);

public:
    DAGOptimizer();
    void clear();
    void processInstruction(const Instruction& inst);
    void processBlock(const vector<Instruction>& instructions);
    vector<string> generateOptimizedTAC();
    vector<string> generateCompactTAC();
    vector<Instruction> generateOptimizedInstructions();

    // Accessors
    const vector<DAGNode>& getNodes() const { return nodes; }
    const vector<string>& getOptimizationLog() const { return optimizationLog; }
    const vector<ProcessedStep>& getSteps() const { return steps; }
    const OptimizationStats& getStats() const { return stats; }
};

#endif // DAGOPTIMIZER_H
