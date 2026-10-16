#if __has_include("BenchmarkRunner.h")
    #include "BenchmarkRunner.h"
    #include "TACParser.h"
    #include "Visualizer.h"
#else
    #include "../include/BenchmarkRunner.h"
    #include "../include/TACParser.h"
    #include "../include/Visualizer.h"
#endif

#include <iostream>

using namespace std;

void BenchmarkRunner::runBenchmark(const string& testName, const string& filepath, DAGOptimizer& optimizer) {
    cout << "\n#######################################################################\n";
    cout << " RUNNING BENCHMARK: " << testName << "\n";
    cout << " File: " << filepath << "\n";
    cout << "#######################################################################\n";

    vector<Instruction> instructions = TACParser::parseFile(filepath);
    if (instructions.empty()) {
        cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    cout << "Original Instructions Read (" << instructions.size() << "):\n";
    for (size_t i = 0; i < instructions.size(); ++i) {
        cout << "  " << (i + 1) << ". " << instructions[i].toString() << "\n";
    }

    optimizer.processBlock(instructions);
    Visualizer::printOptimizationLogs(optimizer);
    Visualizer::printDAG(optimizer);

    optimizer.generateOptimizedTAC();
    Visualizer::printSideBySideComparison(optimizer);
    Visualizer::printPerformanceReport(optimizer);

    string dotName = "dag_" + testName + ".dot";
    Visualizer::exportGraphviz(optimizer, dotName);
}

void BenchmarkRunner::runAllBenchmarks(DAGOptimizer& optimizer) {
    runBenchmark("TestCase1_README", "samples/sample1_readme.tac", optimizer);
    runBenchmark("TestCase2_Commutative", "samples/sample2_commutative.tac", optimizer);
    runBenchmark("TestCase3_Reassignment", "samples/sample3_reassignment.tac", optimizer);
    runBenchmark("TestCase4_ConstantFolding", "samples/sample4_constant_folding.tac", optimizer);
    runBenchmark("TestCase5_MatrixOffset", "samples/sample5_matrix_offset.tac", optimizer);
}
