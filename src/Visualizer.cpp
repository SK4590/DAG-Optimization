#if __has_include("Visualizer.h")
    #include "Visualizer.h"
#else
    #include "../include/Visualizer.h"
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

void Visualizer::exportGraphviz(const DAGOptimizer& optimizer, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        std::cerr << "[ERROR] Could not open " << filename << " for writing Graphviz DOT file.\n";
        return;
    }

    const auto& nodes = optimizer.getNodes();

    dotFile << "digraph DAG_Optimization {\n";
    dotFile << "    rankdir=TB;\n";
    dotFile << "    node [fontname=\"Helvetica\", fontsize=11, shape=record, style=\"filled,rounded\"];\n";
    dotFile << "    edge [fontname=\"Helvetica\", fontsize=10, color=\"#4a5568\"];\n\n";

    for (const auto& node : nodes) {
        std::string color = "#e2e8f0"; // default gray
        std::string labelList = "";
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
    std::cout << "[SUCCESS] Exported DAG visualization to Graphviz file: " << filename << "\n";
}

void Visualizer::printDAG(const DAGOptimizer& optimizer) {
    const auto& nodes = optimizer.getNodes();

    std::cout << "\n===================================================================\n";
    std::cout << "                      CONSTRUCTED DAG NODES                        \n";
    std::cout << "===================================================================\n";
    std::cout << std::left << std::setw(8) << "ID" 
              << std::setw(12) << "Op/Val" 
              << std::setw(10) << "Left" 
              << std::setw(10) << "Right" 
              << std::setw(14) << "Primary Var" 
              << "Active Labels\n";
    std::cout << "-------------------------------------------------------------------\n";
    for (const auto& node : nodes) {
        std::string leftStr = (node.leftChild == -1) ? "-" : std::to_string(node.leftChild);
        std::string rightStr = (node.rightChild == -1) ? "-" : std::to_string(node.rightChild);
        std::string lbls = "";
        for (const auto& l : node.labels) lbls += l + " ";

        std::cout << std::left << std::setw(8)  << node.id 
                  << std::setw(12) << node.op 
                  << std::setw(10) << leftStr 
                  << std::setw(10) << rightStr 
                  << std::setw(14) << node.primaryVar 
                  << lbls << "\n";
    }
    std::cout << "===================================================================\n\n";
}

void Visualizer::printOptimizationLogs(const DAGOptimizer& optimizer) {
    const auto& optimizationLog = optimizer.getOptimizationLog();

    std::cout << "\n--- Optimization Pipeline Audit Log ---\n";
    if (optimizationLog.empty()) {
        std::cout << "No optimizations were triggered.\n";
    } else {
        for (size_t i = 0; i < optimizationLog.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << optimizationLog[i] << "\n";
        }
    }
    std::cout << "---------------------------------------\n\n";
}

void Visualizer::printSideBySideComparison(const DAGOptimizer& optimizer) {
    const auto& steps = optimizer.getSteps();

    std::cout << "\n=======================================================================\n";
    std::cout << "             BEFORE vs AFTER OPTIMIZATION COMPARISON                  \n";
    std::cout << "=======================================================================\n";
    std::cout << std::left << std::setw(35) << "Unoptimized Input (Before)" 
              << " | " << "Optimized TAC (After)\n";
    std::cout << "------------------------------------+----------------------------------\n";
    
    for (const auto& step : steps) {
        std::cout << std::left << std::setw(35) << step.original.toString() 
                  << " | " << step.optimizedTAC << "\n";
    }
    std::cout << "=======================================================================\n\n";
}

void Visualizer::printPerformanceReport(const DAGOptimizer& optimizer) {
    const auto& stats = optimizer.getStats();

    std::cout << "=======================================================================\n";
    std::cout << "               EMPIRICAL PERFORMANCE METRICS SUMMARY                  \n";
    std::cout << "=======================================================================\n";
    std::cout << " - Total Original Instructions        : " << stats.initialInstructions << "\n";
    std::cout << " - Common Subexpressions Eliminated (CSE): " << stats.redundantEliminated << "\n";
    std::cout << " - Commutative Expressions Matched    : " << stats.commutativeMatches << "\n";
    std::cout << " - Constant Foldings Applied           : " << stats.constantFoldings << "\n";
    std::cout << " - Algebraic Identities Simplified    : " << stats.algebraicIdentities << "\n";
    std::cout << " - Copy Assignments Propagated         : " << stats.copiesPropagated << "\n";
    std::cout << " - Total Redundant Computations Cut   : " 
              << (stats.redundantEliminated + stats.constantFoldings + stats.algebraicIdentities) << "\n";
    std::cout << "=======================================================================\n\n";
}
