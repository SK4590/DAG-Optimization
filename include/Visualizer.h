#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include "DAGOptimizer.h"

class Visualizer {
public:
    static void exportGraphviz(const DAGOptimizer& optimizer, const std::string& filename);
    static void printDAG(const DAGOptimizer& optimizer);
    static void printOptimizationLogs(const DAGOptimizer& optimizer);
    static void printSideBySideComparison(const DAGOptimizer& optimizer);
    static void printPerformanceReport(const DAGOptimizer& optimizer);
};

#endif // VISUALIZER_H
