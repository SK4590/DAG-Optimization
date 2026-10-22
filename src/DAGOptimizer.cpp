#if __has_include("DAGOptimizer.h")
    #include "DAGOptimizer.h"
    #include "Utils.h"
    #include "TACParser.h"
#elif __has_include("include/DAGOptimizer.h")
    #include "include/DAGOptimizer.h"
    #include "include/Utils.h"
    #include "include/TACParser.h"
#else
    #include "../include/DAGOptimizer.h"
    #include "../include/Utils.h"
    #include "../include/TACParser.h"
#endif

#include <algorithm>
#include <unordered_set>

using namespace std;

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

string DAGOptimizer::makeSignature(const string& op, int leftId, int rightId) {
    if (Utils::isCommutative(op) && leftId > rightId) {
        swap(leftId, rightId);
        stats.commutativeMatches++;
    }
    return op + "(" + to_string(leftId) + "," + to_string(rightId) + ")";
}

string DAGOptimizer::makeUnarySignature(const string& op, int childId) {
    return op + "(" + to_string(childId) + ")";
}

void DAGOptimizer::detachVariable(const string& varName) {
    if (varToNode.count(varName)) {
        int oldNodeId = varToNode[varName];
        auto& lbls = nodes[oldNodeId].labels;
        lbls.erase(remove(lbls.begin(), lbls.end(), varName), lbls.end());
        varToNode.erase(varName);
    }
}

void DAGOptimizer::attachVariable(const string& varName, int targetNodeId) {
    detachVariable(varName);
    auto& lbls = nodes[targetNodeId].labels;
    if (find(lbls.begin(), lbls.end(), varName) == lbls.end()) {
        lbls.push_back(varName);
    }
    varToNode[varName] = targetNodeId;
    if (nodes[targetNodeId].primaryVar.empty()) {
        nodes[targetNodeId].primaryVar = varName;
    }
}

string DAGOptimizer::getActiveVarForNode(int nodeId, const string& preferredName) const {
    if (nodeId < 0 || nodeId >= (int)nodes.size()) return "";
    const DAGNode& node = nodes[nodeId];
    if (node.isConstant) {
        return Utils::formatNumber(node.constValue);
    }
    // If the preferredName is a temporary alias and the node already has a primary variable,
    // propagate the primary variable to enable dead temporary copy elimination
    if (!node.primaryVar.empty() && node.primaryVar != preferredName) {
        if (!preferredName.empty() && preferredName[0] == 't') {
            return node.primaryVar;
        }
    }
    if (!preferredName.empty() && varToNode.count(preferredName) && varToNode.at(preferredName) == nodeId) {
        return preferredName;
    }
    for (const string& lbl : node.labels) {
        if (varToNode.count(lbl) && varToNode.at(lbl) == nodeId) {
            return lbl;
        }
    }
    if (!node.primaryVar.empty()) return node.primaryVar;
    return node.op;
}

int DAGOptimizer::getOrCreateLeaf(const string& name) {
    double numVal;
    if (Utils::isNumber(name, numVal)) {
        string constKey = Utils::formatNumber(numVal);
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
    string constKey = Utils::formatNumber(val);
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

bool DAGOptimizer::evaluateConstant(const string& op, double v1, double v2, double& result) {
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

int DAGOptimizer::checkAlgebraicIdentity(const string& op, int leftId, int rightId, string& reason) {
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

    // Handle Control Flow Instructions (Labels, Gotos, Conditional Branches)
    if (inst.type == InstructionType::LABEL) {
        step.targetNodeId = -1;
        step.optimizedTAC = inst.toString();
        step.optimizationType = "LABEL";
        steps.push_back(step);
        return;
    }

    if (inst.type == InstructionType::GOTO) {
        step.targetNodeId = -1;
        step.optimizedTAC = inst.toString();
        step.optimizationType = "JUMP";
        steps.push_back(step);
        return;
    }

    if (inst.isBranch()) {
        Instruction optInst = inst;
        if (!inst.arg1.empty() && varToNode.count(inst.arg1)) {
            int nId = varToNode[inst.arg1];
            optInst.arg1 = getActiveVarForNode(nId, inst.arg1);
        }
        if (!inst.arg2.empty() && varToNode.count(inst.arg2)) {
            int nId = varToNode[inst.arg2];
            optInst.arg2 = getActiveVarForNode(nId, inst.arg2);
        }
        step.targetNodeId = -1;
        step.optimizedTAC = optInst.toString();
        step.optimizationType = "BRANCH";
        steps.push_back(step);
        return;
    }

    // 1. Copy / Direct Constant assignment: res = arg1
    if (inst.type == InstructionType::COPY || inst.type == InstructionType::CONSTANT) {
        int sourceId = getOrCreateLeaf(inst.arg1);
        attachVariable(inst.result, sourceId);
        stats.copiesPropagated++;

        string srcVar = getActiveVarForNode(sourceId, inst.arg1);
        step.targetNodeId = sourceId;
        step.optimizedTAC = inst.result + " = " + srcVar;
        step.optimizationType = "COPY_PROPAGATION";
        steps.push_back(step);

        optimizationLog.push_back("COPY PROPAGATION: '" + inst.result + "' aliases Node " + 
                                 to_string(sourceId) + " (" + srcVar + ")");
        return;
    }

    // 2. Unary Operation: res = op arg1
    if (inst.type == InstructionType::UNARY) {
        int childId = getOrCreateLeaf(inst.arg1);
        string childVar = getActiveVarForNode(childId, inst.arg1);

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

        string sig = makeUnarySignature(inst.op, childId);
        if (valueNumbering.count(sig)) {
            int existingId = valueNumbering[sig];
            string repVar = nodes[existingId].primaryVar;
            attachVariable(inst.result, existingId);
            stats.redundantEliminated++;

            step.targetNodeId = existingId;
            step.optimizedTAC = inst.result + " = " + repVar;
            step.optimizationType = "CSE";
            steps.push_back(step);

            optimizationLog.push_back("CSE (UNARY): '" + inst.result + "' reused Node " + 
                                     to_string(existingId) + " (" + repVar + ")");
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
        string leftVar = getActiveVarForNode(leftId, inst.arg1);
        string rightVar = getActiveVarForNode(rightId, inst.arg2);

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
        string identityReason;
        int simplifiedNodeId = checkAlgebraicIdentity(inst.op, leftId, rightId, identityReason);
        if (simplifiedNodeId != -1) {
            attachVariable(inst.result, simplifiedNodeId);
            stats.algebraicIdentities++;

            string repVar = getActiveVarForNode(simplifiedNodeId);
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
        string sig = makeSignature(inst.op, leftId, rightId);
        if (valueNumbering.count(sig)) {
            int existingId = valueNumbering[sig];
            string repVar = nodes[existingId].primaryVar;
            attachVariable(inst.result, existingId);
            stats.redundantEliminated++;

            step.targetNodeId = existingId;
            step.optimizedTAC = inst.result + " = " + repVar;
            step.optimizationType = "CSE";
            steps.push_back(step);

            optimizationLog.push_back("CSE (COMMON SUBEXPRESSION): '" + inst.result + " = " + 
                                     inst.arg1 + " " + inst.op + " " + inst.arg2 + 
                                     "' maps to existing Node " + to_string(existingId) + 
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

void DAGOptimizer::processBlock(const vector<Instruction>& instructions) {
    clear();
    for (const auto& inst : instructions) {
        processInstruction(inst);
    }
}

vector<string> DAGOptimizer::generateOptimizedTAC() {
    vector<string> optimizedTAC;
    for (const auto& step : steps) {
        optimizedTAC.push_back(step.optimizedTAC);
    }
    stats.finalInstructions = optimizedTAC.size();
    return optimizedTAC;
}

vector<string> DAGOptimizer::generateCompactTAC() {
    vector<string> compact;
    for (const auto& step : steps) {
        if (step.optimizationType == "CSE" && step.original.result[0] == 't') {
            compact.push_back("// " + step.original.result + " eliminated (" + step.optimizedTAC + ")");
        } else {
            compact.push_back(step.optimizedTAC);
        }
    }
    return compact;
}

vector<Instruction> DAGOptimizer::generateOptimizedInstructions() {
    vector<Instruction> result;
    for (const auto& step : steps) {
        Instruction inst;
        if (TACParser::parseLine(step.optimizedTAC, inst)) {
            result.push_back(inst);
        } else {
            result.push_back(step.original);
        }
    }
    return result;
}

