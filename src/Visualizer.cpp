#if __has_include("Visualizer.h")
    #include "Visualizer.h"
#else
    #include "../include/Visualizer.h"
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

void Visualizer::exportGraphviz(const DAGOptimizer& optimizer, const string& filename) {
    ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        cerr << "[ERROR] Could not open " << filename << " for writing Graphviz DOT file.\n";
        return;
    }

    const auto& nodes = optimizer.getNodes();

    dotFile << "digraph DAG_Optimization {\n";
    dotFile << "    rankdir=TB;\n";
    dotFile << "    node [fontname=\"Helvetica\", fontsize=11, shape=record, style=\"filled,rounded\"];\n";
    dotFile << "    edge [fontname=\"Helvetica\", fontsize=10, color=\"#4a5568\"];\n\n";

    for (const auto& node : nodes) {
        string color = "#e2e8f0"; // default gray
        string labelList = "";
        for (size_t i = 0; i < node.labels.size(); ++i) {
            if (i > 0) labelList += ", ";
            labelList += node.labels[i];
        }

        if (node.isConstant) {
            color = "#fef3c7"; // light yellow
        } else if (node.leftChild == -1 && node.rightChild == -1) {
            color = "#e0f2fe"; // light blue leaf
        } else {
            color = "#dcfce7"; // light green computation
        }

        dotFile << "    node_" << node.id << " [label=\"{Node " << node.id 
                << ": '" << node.op << "' | Labels: [" << labelList << "]}\", fillcolor=\"" 
                << color << "\"];\n";

        if (node.leftChild != -1) {
            dotFile << "    node_" << node.id << " -> node_" << node.leftChild 
                    << " [label=\"L\"];\n";
        }
        if (node.rightChild != -1) {
            dotFile << "    node_" << node.id << " -> node_" << node.rightChild 
                    << " [label=\"R\"];\n";
        }
    }

    dotFile << "}\n";
    dotFile.close();
    cout << "[SUCCESS] Exported DAG visualization to Graphviz file: " << filename << "\n";
}

void Visualizer::printDAG(const DAGOptimizer& optimizer) {
    const auto& nodes = optimizer.getNodes();

    cout << "\n===================================================================\n";
    cout << "                      CONSTRUCTED DAG NODES                        \n";
    cout << "===================================================================\n";
    cout << left << setw(8) << "ID" 
         << setw(12) << "Op/Val" 
         << setw(10) << "Left" 
         << setw(10) << "Right" 
         << setw(14) << "Primary Var" 
         << "Active Labels\n";
    cout << "-------------------------------------------------------------------\n";
    for (const auto& node : nodes) {
        string leftStr = (node.leftChild == -1) ? "-" : to_string(node.leftChild);
        string rightStr = (node.rightChild == -1) ? "-" : to_string(node.rightChild);
        string lbls = "";
        for (const auto& l : node.labels) lbls += l + " ";

        cout << left << setw(8)  << node.id 
             << setw(12) << node.op 
             << setw(10) << leftStr 
             << setw(10) << rightStr 
             << setw(14) << node.primaryVar 
             << lbls << "\n";
    }
    cout << "===================================================================\n\n";
}

void Visualizer::printOptimizationLogs(const DAGOptimizer& optimizer) {
    const auto& optimizationLog = optimizer.getOptimizationLog();

    cout << "\n--- Optimization Pipeline Audit Log ---\n";
    if (optimizationLog.empty()) {
        cout << "No optimizations were triggered.\n";
    } else {
        for (size_t i = 0; i < optimizationLog.size(); ++i) {
            cout << "  " << (i + 1) << ". " << optimizationLog[i] << "\n";
        }
    }
    cout << "---------------------------------------\n\n";
}

void Visualizer::printSideBySideComparison(const DAGOptimizer& optimizer) {
    const auto& steps = optimizer.getSteps();

    cout << "\n=======================================================================\n";
    cout << "             BEFORE vs AFTER OPTIMIZATION COMPARISON                  \n";
    cout << "=======================================================================\n";
    cout << left << setw(35) << "Unoptimized Input (Before)" 
         << " | " << "Optimized TAC (After)\n";
    cout << "------------------------------------+----------------------------------\n";
    
    for (const auto& step : steps) {
        cout << left << setw(35) << step.original.toString() 
             << " | " << step.optimizedTAC << "\n";
    }
    cout << "=======================================================================\n\n";
}

void Visualizer::printPerformanceReport(const DAGOptimizer& optimizer) {
    const auto& stats = optimizer.getStats();

    cout << "=======================================================================\n";
    cout << "               EMPIRICAL PERFORMANCE METRICS SUMMARY                  \n";
    cout << "=======================================================================\n";
    cout << " - Total Original Instructions        : " << stats.initialInstructions << "\n";
    cout << " - Common Subexpressions Eliminated (CSE): " << stats.redundantEliminated << "\n";
    cout << " - Commutative Expressions Matched    : " << stats.commutativeMatches << "\n";
    cout << " - Constant Foldings Applied           : " << stats.constantFoldings << "\n";
    cout << " - Algebraic Identities Simplified    : " << stats.algebraicIdentities << "\n";
    cout << " - Copy Assignments Propagated         : " << stats.copiesPropagated << "\n";
    cout << " - Total Redundant Computations Cut   : " 
         << (stats.redundantEliminated + stats.constantFoldings + stats.algebraicIdentities) << "\n";
    cout << "=======================================================================\n\n";
}
