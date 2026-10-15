#if __has_include("DAGOptimizer.h")
    #include "DAGOptimizer.h"
    #include "Utils.h"
#else
    #include "../include/DAGOptimizer.h"
    #include "../include/Utils.h"
#endif

#include <algorithm>
#include <unordered_set>

DAGOptimizer::DAGOptimizer() {
    clear();
}

void DAGOptimizer::clear() {
    nodes.clear();
    valueNumbering.clear();
    varToNode.clear();
    constToNode.clear();
    optimizationLog.clear();
    steps.clear();
    stats = OptimizationStats();
}

std::string DAGOptimizer::makeSignature(const std::string& op, int leftId, int rightId) {
    if (Utils::isCommutative(op) && leftId > rightId) {
        std::swap(leftId, rightId);
        stats.commutativeMatches++;
    }
    return op + "(" + std::to_string(leftId) + "," + std::to_string(rightId) + ")";
}

std::string DAGOptimizer::makeUnarySignature(const std::string& op, int childId) {
    return op + "(" + std::to_string(childId) + ")";
}

void DAGOptimizer::detachVariable(const std::string& varName) {
    if (varToNode.count(varName)) {
        int oldNodeId = varToNode[varName];
        auto& lbls = nodes[oldNodeId].labels;
        lbls.erase(std::remove(lbls.begin(), lbls.end(), varName), lbls.end());
        varToNode.erase(varName);
    }
}

void DAGOptimizer::attachVariable(const std::string& varName, int targetNodeId) {
    detachVariable(varName);
    auto& lbls = nodes[targetNodeId].labels;
    if (std::find(lbls.begin(), lbls.end(), varName) == lbls.end()) {
        lbls.push_back(varName);
    }
    varToNode[varName] = targetNodeId;
    if (nodes[targetNodeId].primaryVar.empty()) {
        nodes[targetNodeId].primaryVar = varName;
    }
}

std::string DAGOptimizer::getActiveVarForNode(int nodeId, const std::string& preferredName) const {
    if (nodeId < 0 || nodeId >= (int)nodes.size()) return "";
    const DAGNode& node = nodes[nodeId];
    if (node.isConstant) {
        return Utils::formatNumber(node.constValue);
    }
    if (!preferredName.empty() && varToNode.count(preferredName) && varToNode.at(preferredName) == nodeId) {
        return preferredName;
    }
    for (const std::string& lbl : node.labels) {
        if (varToNode.count(lbl) && varToNode.at(lbl) == nodeId) {
            return lbl;
        }
    }
    if (!node.primaryVar.empty()) return node.primaryVar;
    return node.op;
}

int DAGOptimizer::getOrCreateLeaf(const std::string& name) {
    double numVal;
    if (Utils::isNumber(name, numVal)) {
        std::string constKey = Utils::formatNumber(numVal);
        if (constToNode.count(constKey)) {
            return constToNode[constKey];
        }
        int id = nodes.size();
        DAGNode leaf;
        leaf.id = id;
        leaf.op = constKey;
        leaf.leftChild = -1;
        leaf.rightChild = -1;
        leaf.isConstant = true;
        leaf.constValue = numVal;
        leaf.primaryVar = constKey;
        leaf.labels = {constKey};
        leaf.creationComment = "Constant Leaf: " + constKey;
        nodes.push_back(leaf);
        constToNode[constKey] = id;
        return id;
    }

    if (varToNode.count(name)) {
        return varToNode[name];
    }

    int id = nodes.size();
    DAGNode leaf;
    leaf.id = id;
    leaf.op = name;
    leaf.leftChild = -1;
    leaf.rightChild = -1;
    leaf.isConstant = false;
    leaf.constValue = 0.0;
    leaf.primaryVar = name;
    leaf.labels = {name};
    leaf.creationComment = "Variable Leaf: " + name;
    nodes.push_back(leaf);
    varToNode[name] = id;
    return id;
}

int DAGOptimizer::getOrCreateConstantNode(double val) {
    std::string constKey = Utils::formatNumber(val);
    if (constToNode.count(constKey)) {
        return constToNode[constKey];
    }
    int id = nodes.size();
    DAGNode leaf;
    leaf.id = id;
    leaf.op = constKey;
    leaf.leftChild = -1;
    leaf.rightChild = -1;
    leaf.isConstant = true;
    leaf.constValue = val;
    leaf.primaryVar = constKey;
    leaf.labels = {constKey};
    leaf.creationComment = "Folded Constant: " + constKey;
    nodes.push_back(leaf);
    constToNode[constKey] = id;
    return id;
}

bool DAGOptimizer::evaluateConstant(const std::string& op, double v1, double v2, double& result) {
    if (op == "+") { result = v1 + v2; return true; }
    if (op == "-") { result = v1 - v2; return true; }
    if (op == "*") { result = v1 * v2; return true; }
    if (op == "/") {
        if (v2 == 0.0) return false;
        result = v1 / v2;
        return true;
    }
    if (op == "%") {
        if (v2 == 0.0) return false;
        result = (long long)v1 % (long long)v2;
        return true;
    }
    if (op == "==") { result = (v1 == v2) ? 1.0 : 0.0; return true; }
    if (op == "!=") { result = (v1 != v2) ? 1.0 : 0.0; return true; }
    if (op == "<")  { result = (v1 < v2)  ? 1.0 : 0.0; return true; }
    if (op == ">")  { result = (v1 > v2)  ? 1.0 : 0.0; return true; }
    if (op == "<=") { result = (v1 <= v2) ? 1.0 : 0.0; return true; }
    if (op == ">=") { result = (v1 >= v2) ? 1.0 : 0.0; return true; }
    return false;
}

int DAGOptimizer::checkAlgebraicIdentity(const std::string& op, int leftId, int rightId, std::string& reason) {
    const DAGNode& left = nodes[leftId];
    const DAGNode& right = nodes[rightId];

    if (op == "+") {
        if (right.isConstant && right.constValue == 0.0) {
            reason = "Identity x + 0 = x";
            return leftId;
        }
        if (left.isConstant && left.constValue == 0.0) {
            reason = "Identity 0 + x = x";
            return rightId;
        }
    } else if (op == "-") {
        if (right.isConstant && right.constValue == 0.0) {
            reason = "Identity x - 0 = x";
            return leftId;
        }
        if (leftId == rightId) {
            reason = "Identity x - x = 0";
            return getOrCreateConstantNode(0.0);
        }
    } else if (op == "*") {
        if (right.isConstant && right.constValue == 1.0) {
            reason = "Identity x * 1 = x";
            return leftId;
        }
        if (left.isConstant && left.constValue == 1.0) {
            reason = "Identity 1 * x = x";
            return rightId;
        }
        if ((right.isConstant && right.constValue == 0.0) || (left.isConstant && left.constValue == 0.0)) {
            reason = "Identity x * 0 = 0";
            return getOrCreateConstantNode(0.0);
        }
    } else if (op == "/") {
        if (right.isConstant && right.constValue == 1.0) {
            reason = "Identity x / 1 = x";
            return leftId;
        }
        if (leftId == rightId) {
            reason = "Identity x / x = 1";
            return getOrCreateConstantNode(1.0);
        }
    }
    return -1;
}

void DAGOptimizer::processInstruction(const Instruction& inst) {
    stats.initialInstructions++;
    ProcessedStep step;
    step.original = inst;

    // 1. Copy / Direct Constant assignment: res = arg1
    if (inst.type == InstructionType::COPY || inst.type == InstructionType::CONSTANT) {
        int sourceId = getOrCreateLeaf(inst.arg1);
        attachVariable(inst.result, sourceId);
        stats.copiesPropagated++;

        std::string srcVar = getActiveVarForNode(sourceId, inst.arg1);
        step.targetNodeId = sourceId;
        step.optimizedTAC = inst.result + " = " + srcVar;
        step.optimizationType = "COPY_PROPAGATION";
        steps.push_back(step);

        optimizationLog.push_back("COPY PROPAGATION: '" + inst.result + "' aliases Node " + 
                                 std::to_string(sourceId) + " (" + srcVar + ")");
        return;
    }

    // 2. Unary Operation: res = op arg1
    if (inst.type == InstructionType::UNARY) {
        int childId = getOrCreateLeaf(inst.arg1);
        std::string childVar = getActiveVarForNode(childId, inst.arg1);

        if (nodes[childId].isConstant) {
            double resVal;
            if (inst.op == "-") resVal = -nodes[childId].constValue;
            else if (inst.op == "!") resVal = (nodes[childId].constValue == 0.0) ? 1.0 : 0.0;
            else resVal = nodes[childId].constValue;

            int foldedId = getOrCreateConstantNode(resVal);
            attachVariable(inst.result, foldedId);
            stats.constantFoldings++;

            step.targetNodeId = foldedId;
            step.optimizedTAC = inst.result + " = " + Utils::formatNumber(resVal);
            step.optimizationType = "CONSTANT_FOLDING";
            steps.push_back(step);

            optimizationLog.push_back("CONSTANT FOLDING: '" + inst.result + " = " + inst.op + 
                                     inst.arg1 + "' evaluated to " + Utils::formatNumber(resVal));
            return;
        }

        std::string sig = makeUnarySignature(inst.op, childId);
        if (valueNumbering.count(sig)) {
            int existingId = valueNumbering[sig];
            std::string repVar = nodes[existingId].primaryVar;
            attachVariable(inst.result, existingId);
            stats.redundantEliminated++;

            step.targetNodeId = existingId;
            step.optimizedTAC = inst.result + " = " + repVar;
            step.optimizationType = "CSE";
            steps.push_back(step);

            optimizationLog.push_back("CSE (UNARY): '" + inst.result + "' reused Node " + 
                                     std::to_string(existingId) + " (" + repVar + ")");
        } else {
            int newId = nodes.size();
            DAGNode newNode;
            newNode.id = newId;
            newNode.op = inst.op;
            newNode.leftChild = childId;
            newNode.rightChild = -1;
            newNode.isConstant = false;
            newNode.constValue = 0.0;
            newNode.primaryVar = inst.result;
            newNode.creationComment = "Unary Computation: " + inst.op + " " + childVar;
            nodes.push_back(newNode);
            valueNumbering[sig] = newId;
            attachVariable(inst.result, newId);

            step.targetNodeId = newId;
            step.optimizedTAC = inst.result + " = " + inst.op + " " + childVar;
            step.optimizationType = "NEW_COMPUTATION";
            steps.push_back(step);
        }
        return;
    }

    // 3. Binary Operation: res = arg1 op arg2
    if (inst.type == InstructionType::BINARY) {
        int leftId = getOrCreateLeaf(inst.arg1);
        int rightId = getOrCreateLeaf(inst.arg2);
        std::string leftVar = getActiveVarForNode(leftId, inst.arg1);
        std::string rightVar = getActiveVarForNode(rightId, inst.arg2);

        // A. Constant Folding Check
        if (nodes[leftId].isConstant && nodes[rightId].isConstant) {
            double computed = 0.0;
            if (evaluateConstant(inst.op, nodes[leftId].constValue, nodes[rightId].constValue, computed)) {
                int foldedId = getOrCreateConstantNode(computed);
                attachVariable(inst.result, foldedId);
                stats.constantFoldings++;

                step.targetNodeId = foldedId;
                step.optimizedTAC = inst.result + " = " + Utils::formatNumber(computed);
                step.optimizationType = "CONSTANT_FOLDING";
                steps.push_back(step);

                optimizationLog.push_back("CONSTANT FOLDING: '" + inst.result + " = " + 
                                         Utils::formatNumber(nodes[leftId].constValue) + " " + inst.op + " " + 
                                         Utils::formatNumber(nodes[rightId].constValue) + 
                                         "' folded to " + Utils::formatNumber(computed));
                return;
            }
        }

        // B. Algebraic Identities Check
        std::string identityReason;
        int simplifiedNodeId = checkAlgebraicIdentity(inst.op, leftId, rightId, identityReason);
        if (simplifiedNodeId != -1) {
            attachVariable(inst.result, simplifiedNodeId);
            stats.algebraicIdentities++;

            std::string repVar = getActiveVarForNode(simplifiedNodeId);
            step.targetNodeId = simplifiedNodeId;
            step.optimizedTAC = inst.result + " = " + repVar;
            step.optimizationType = "ALGEBRAIC_IDENTITY";
            steps.push_back(step);

            optimizationLog.push_back("ALGEBRAIC SIMPLIFICATION: '" + inst.result + " = " + 
                                     inst.arg1 + " " + inst.op + " " + inst.arg2 + "' -> " + 
                                     identityReason + " (" + repVar + ")");
            return;
        }

        // C. Common Subexpression Elimination (CSE) via Value Numbering
        std::string sig = makeSignature(inst.op, leftId, rightId);
        if (valueNumbering.count(sig)) {
            int existingId = valueNumbering[sig];
            std::string repVar = nodes[existingId].primaryVar;
            attachVariable(inst.result, existingId);
            stats.redundantEliminated++;

            step.targetNodeId = existingId;
            step.optimizedTAC = inst.result + " = " + repVar;
            step.optimizationType = "CSE";
            steps.push_back(step);

            optimizationLog.push_back("CSE (COMMON SUBEXPRESSION): '" + inst.result + " = " + 
                                     inst.arg1 + " " + inst.op + " " + inst.arg2 + 
                                     "' maps to existing Node " + std::to_string(existingId) + 
                                     " [Represented by " + repVar + "]");
        } else {
            int newId = nodes.size();
            DAGNode newNode;
            newNode.id = newId;
            newNode.op = inst.op;
            newNode.leftChild = leftId;
            newNode.rightChild = rightId;
            newNode.isConstant = false;
            newNode.constValue = 0.0;
            newNode.primaryVar = inst.result;
            newNode.creationComment = "Binary Computation: " + leftVar + " " + inst.op + " " + rightVar;
            nodes.push_back(newNode);
            valueNumbering[sig] = newId;
            attachVariable(inst.result, newId);

            step.targetNodeId = newId;
            step.optimizedTAC = inst.result + " = " + leftVar + " " + inst.op + " " + rightVar;
            step.optimizationType = "NEW_COMPUTATION";
            steps.push_back(step);
        }
    }
}

void DAGOptimizer::processBlock(const std::vector<Instruction>& instructions) {
    clear();
    for (const auto& inst : instructions) {
        processInstruction(inst);
    }
}

std::vector<std::string> DAGOptimizer::generateOptimizedTAC() {
    std::vector<std::string> optimizedTAC;
    for (const auto& step : steps) {
        optimizedTAC.push_back(step.optimizedTAC);
    }
    stats.finalInstructions = optimizedTAC.size();
    return optimizedTAC;
}

std::vector<std::string> DAGOptimizer::generateCompactTAC() {
    std::vector<std::string> compact;
    for (const auto& step : steps) {
        if (step.optimizationType == "CSE" && step.original.result[0] == 't') {
            compact.push_back("// " + step.original.result + " eliminated (" + step.optimizedTAC + ")");
        } else {
            compact.push_back(step.optimizedTAC);
        }
    }
    return compact;
}
