#if __has_include("Visualizer.h")
    #include "Visualizer.h"
    #include "CFG.h"
#elif __has_include("include/Visualizer.h")
    #include "include/Visualizer.h"
    #include "include/CFG.h"
#else
    #include "../include/Visualizer.h"
    #include "../include/CFG.h"
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

// Helper to escape record string characters for Graphviz
static string escapeRecordString(const string& s) {
    string res = "";
    for (char c : s) {
        if (c == '"') res += "\\\"";
        else if (c == '\\') res += "\\\\";
        else if (c == '{') res += "\\{";
        else if (c == '}') res += "\\}";
        else if (c == '<') res += "\\<";
        else if (c == '>') res += "\\>";
        else if (c == '|') res += "\\|";
        else res += c;
    }
    return res;
}

void Visualizer::exportCFG(const CFG& cfg, const string& filename) {
    ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        cerr << "[ERROR] Could not open " << filename << " for writing CFG DOT file.\n";
        return;
    }

    const auto& blocks = cfg.getBlocks();
    const auto& edges = cfg.getEdges();

    dotFile << "digraph CFG_Optimization {\n";
    dotFile << "    rankdir=TB;\n";
    dotFile << "    node [fontname=\"Consolas\", fontsize=10, shape=record, style=\"filled,rounded\"];\n";
    dotFile << "    edge [fontname=\"Helvetica\", fontsize=9];\n\n";

    for (const auto& b : blocks) {
        string header = "Block " + b.name;
        vector<string> labels = b.getLabels();
        if (!labels.empty()) {
            header += " (" + labels[0];
            for (size_t i = 1; i < labels.size(); ++i) header += ", " + labels[i];
            header += ":)";
        }

        string instText = "";
        if (b.instructions.empty()) {
            instText = "  <empty block>\\l";
        } else {
            for (const auto& inst : b.instructions) {
                instText += "  " + escapeRecordString(inst.toString()) + "\\l";
            }
        }

        // Live sets formatting
        string inSet = "{";
        int cnt = 0;
        vector<string> sortedIn(b.liveIn.begin(), b.liveIn.end());
        sort(sortedIn.begin(), sortedIn.end());
        for (const auto& v : sortedIn) {
            if (cnt++ > 0) inSet += ", ";
            inSet += v;
        }
        inSet += "}";

        string outSet = "{";
        cnt = 0;
        vector<string> sortedOut(b.liveOut.begin(), b.liveOut.end());
        sort(sortedOut.begin(), sortedOut.end());
        for (const auto& v : sortedOut) {
            if (cnt++ > 0) outSet += ", ";
            outSet += v;
        }
        outSet += "}";

        string fill = "#f8fafc";
        string border = "#64748b";
        if (b.id == 0) {
            fill = "#e0f2fe"; // Light blue entry block
            border = "#0284c7";
        } else if (b.successors.empty()) {
            fill = "#fef2f2"; // Light rose exit block
            border = "#e11d48";
        }

        dotFile << "    node_" << b.id << " [label=\"{ " << header 
                << " | " << instText 
                << " | IN: " << escapeRecordString(inSet) << "\\lOUT: " << escapeRecordString(outSet) << "\\l }\", "
                << "fillcolor=\"" << fill << "\", color=\"" << border << "\", penwidth=1.5];\n";
    }

    dotFile << "\n";

    for (const auto& edge : edges) {
        string color = "#475569";
        string style = "solid";
        string fontColor = "#475569";
        double penwidth = 1.2;

        if (edge.type == EdgeType::TRUE_BRANCH) {
            color = "#16a34a";
            fontColor = "#16a34a";
            penwidth = 1.6;
        } else if (edge.type == EdgeType::FALSE_BRANCH) {
            color = "#dc2626";
            fontColor = "#dc2626";
            penwidth = 1.6;
        } else if (edge.type == EdgeType::LOOP_BACK) {
            color = "#2563eb";
            fontColor = "#2563eb";
            style = "dashed";
            penwidth = 1.6;
        }

        dotFile << "    node_" << edge.from << " -> node_" << edge.to
                << " [label=\" " << edge.label << "\", color=\"" << color 
                << "\", fontcolor=\"" << fontColor 
                << "\", style=\"" << style 
                << "\", penwidth=" << penwidth << "];\n";
    }

    dotFile << "}\n";
    dotFile.close();
    cout << "[SUCCESS] Exported CFG visualization to Graphviz file: " << filename << "\n";
}

void Visualizer::printCFG(const CFG& cfg) {
    const auto& blocks = cfg.getBlocks();
    cout << "\n=======================================================================\n";
    cout << "                    CONTROL FLOW GRAPH (CFG) TOPOLOGY                 \n";
    cout << "=======================================================================\n";
    for (const auto& b : blocks) {
        string labelsStr = "";
        for (const auto& l : b.getLabels()) labelsStr += l + ": ";

        cout << "--- Basic Block: " << b.name << " " << (labelsStr.empty() ? "" : "(" + labelsStr + ")") << "\n";
        
        cout << "  Predecessors: ";
        if (b.predecessors.empty()) cout << "[None (Entry)]";
        else {
            for (size_t i = 0; i < b.predecessors.size(); ++i) {
                if (i > 0) cout << ", ";
                cout << "B" << b.predecessors[i];
            }
        }
        cout << "\n";

        cout << "  Successors  : ";
        if (b.successors.empty()) cout << "[None (Exit)]";
        else {
            for (size_t i = 0; i < b.successors.size(); ++i) {
                if (i > 0) cout << ", ";
                cout << "B" << b.successors[i];
            }
        }
        cout << "\n";

        cout << "  Instructions (" << b.instructions.size() << "):\n";
        for (size_t i = 0; i < b.instructions.size(); ++i) {
            cout << "    " << (i + 1) << ". " << b.instructions[i].toString() << "\n";
        }
        cout << "\n";
    }
    cout << "=======================================================================\n\n";
}

static string formatSet(const unordered_set<string>& s) {
    vector<string> sortedVec(s.begin(), s.end());
    sort(sortedVec.begin(), sortedVec.end());
    string res = "{";
    for (size_t i = 0; i < sortedVec.size(); ++i) {
        if (i > 0) res += ", ";
        res += sortedVec[i];
    }
    res += "}";
    return res;
}

void Visualizer::printLVAReport(const CFG& cfg) {
    const auto& blocks = cfg.getBlocks();
    cout << "\n===================================================================================================\n";
    cout << "                             GLOBAL LIVE VARIABLE ANALYSIS (LVA) REPORT                            \n";
    cout << "===================================================================================================\n";
    cout << left << setw(8)  << "Block"
         << setw(24) << "GEN[B]"
         << setw(24) << "KILL[B]"
         << setw(24) << "IN[B] (LiveIn)"
         << "OUT[B] (LiveOut)\n";
    cout << "---------------------------------------------------------------------------------------------------\n";
    for (const auto& b : blocks) {
        cout << left << setw(8)  << b.name
             << setw(24) << formatSet(b.gen)
             << setw(24) << formatSet(b.kill)
             << setw(24) << formatSet(b.liveIn)
             << formatSet(b.liveOut) << "\n";
    }
    cout << "===================================================================================================\n\n";
}

void Visualizer::printMultiBlockComparison(const CFG& initialCfg, const CFG& optimizedCfg, int deadEliminated) {
    auto initialInsts = initialCfg.getAllInstructions();
    auto finalInsts = optimizedCfg.getAllInstructions();

    cout << "\n=======================================================================\n";
    cout << "             CFG BEFORE vs AFTER FULL OPTIMIZATION PIPELINE            \n";
    cout << "=======================================================================\n";
    cout << left << setw(35) << "Original Program TAC" 
         << " | " << "Fully Optimized Program TAC\n";
    cout << "------------------------------------+----------------------------------\n";

    size_t maxRows = max(initialInsts.size(), finalInsts.size());
    for (size_t i = 0; i < maxRows; ++i) {
        string orig = (i < initialInsts.size()) ? initialInsts[i].toString() : "";
        string opt = (i < finalInsts.size()) ? finalInsts[i].toString() : "";
        cout << left << setw(35) << orig << " | " << opt << "\n";
    }
    cout << "=======================================================================\n\n";

    cout << "=======================================================================\n";
    cout << "                         OPTIMIZATION METRICS SUMMARY                  \n";
    cout << "=======================================================================\n";
    cout << " - Total Basic Blocks in CFG           : " << initialCfg.getBlockCount() << "\n";
    cout << " - Total Directed Control Edges        : " << initialCfg.getEdges().size() << "\n";
    cout << " - Initial Instruction Count           : " << initialInsts.size() << "\n";
    cout << " - Dead Temporary Assignments Cut (DCE): " << deadEliminated << "\n";
    cout << " - Final Optimized Instruction Count   : " << finalInsts.size() << "\n";
    if (!initialInsts.empty()) {
        double redPct = ((double)(initialInsts.size() - finalInsts.size()) / initialInsts.size()) * 100.0;
        cout << " - Net Program Size Reduction Rate     : " << fixed << setprecision(1) << redPct << "%\n";
    }
    cout << "=======================================================================\n\n";
}
