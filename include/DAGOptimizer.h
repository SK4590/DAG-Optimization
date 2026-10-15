#ifndef DAGOPTIMIZER_H
#define DAGOPTIMIZER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "Instruction.h"
#include "DAGNode.h"

class DAGOptimizer {
private:
    std::vector<DAGNode> nodes;
    std::unordered_map<std::string, int> valueNumbering; // Signature -> Node ID
    std::unordered_map<std::string, int> varToNode;      // Variable Name -> Node ID
    std::unordered_map<std::string, int> constToNode;    // Formatted Const String -> Node ID
    std::vector<std::string> optimizationLog;
    std::vector<ProcessedStep> steps;
    OptimizationStats stats;

    std::string makeSignature(const std::string& op, int leftId, int rightId);
    std::string makeUnarySignature(const std::string& op, int childId);
    void detachVariable(const std::string& varName);
    void attachVariable(const std::string& varName, int targetNodeId);
    std::string getActiveVarForNode(int nodeId, const std::string& preferredName = "") const;
    int getOrCreateLeaf(const std::string& name);
    int getOrCreateConstantNode(double val);
    bool evaluateConstant(const std::string& op, double v1, double v2, double& result);
    int checkAlgebraicIdentity(const std::string& op, int leftId, int rightId, std::string& reason);

public:
    DAGOptimizer();
    void clear();
    void processInstruction(const Instruction& inst);
    void processBlock(const std::vector<Instruction>& instructions);
    std::vector<std::string> generateOptimizedTAC();
    std::vector<std::string> generateCompactTAC();

    // Accessors
    const std::vector<DAGNode>& getNodes() const { return nodes; }
    const std::vector<std::string>& getOptimizationLog() const { return optimizationLog; }
    const std::vector<ProcessedStep>& getSteps() const { return steps; }
    const OptimizationStats& getStats() const { return stats; }
};

#endif // DAGOPTIMIZER_H
