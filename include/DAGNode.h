#ifndef DAGNODE_H
#define DAGNODE_H

#include <string>
#include <vector>
#include "Instruction.h"

using namespace std;

// ============================================================================
// DAG Node Structure
// ============================================================================
struct DAGNode {
    int id;
    string op;                  // Operator for internal node, or var/const name for leaf
    int leftChild;                   // -1 for leaf or unary
    int rightChild;                  // -1 for leaf or unary
    vector<string> labels; // Active variables currently referencing this node's value
    bool isConstant;                 // True if compile-time constant
    double constValue;               // Numerical value if constant
    string primaryVar;          // Representative variable first assigned to this node
    string creationComment;     // Audit message for optimizations applied
};

// ============================================================================
// Processed Step Record
// ============================================================================
struct ProcessedStep {
    Instruction original;
    string optimizedTAC;
    string optimizationType;
    int targetNodeId;
};

// ============================================================================
// Optimization Statistics
// ============================================================================
struct OptimizationStats {
    int initialInstructions = 0;
    int finalInstructions = 0;
    int redundantEliminated = 0;
    int commutativeMatches = 0;
    int constantFoldings = 0;
    int algebraicIdentities = 0;
    int copiesPropagated = 0;

    double getReductionPercentage() const {
        if (initialInstructions == 0) return 0.0;
        return ((double)(initialInstructions - finalInstructions) / initialInstructions) * 100.0;
    }
};

#endif // DAGNODE_H
