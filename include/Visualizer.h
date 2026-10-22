#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include "Instruction.h"
#include "DAGOptimizer.h"

using namespace std;

class CFG; // Forward declaration

class Visualizer {
public:
    static void exportGraphviz(const DAGOptimizer& optimizer, const string& filename);
    static void exportCFG(const CFG& cfg, const string& filename);
    static void printDAG(const DAGOptimizer& optimizer);
    static void printOptimizationLogs(const DAGOptimizer& optimizer);
    static void printSideBySideComparison(const DAGOptimizer& optimizer);
    static void printPerformanceReport(const DAGOptimizer& optimizer);
    static void printCFG(const CFG& cfg);
    static void printLVAReport(const CFG& cfg);
    static void printMultiBlockComparison(const CFG& initialCfg, const CFG& optimizedCfg, int deadEliminated);
    static void printThreeWayComparison(const vector<Instruction>& origInsts,
                                        const vector<Instruction>& optInsts,
                                        const vector<string>& asmCode,
                                        int stackFrameSize,
                                        int spills);
};

#endif // VISUALIZER_H
