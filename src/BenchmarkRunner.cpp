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

void BenchmarkRunner::runBenchmark(const std::string& testName, const std::string& filepath, DAGOptimizer& optimizer) {
    std::cout << "\n#######################################################################\n";
    std::cout << " RUNNING BENCHMARK: " << testName << "\n";
    std::cout << " File: " << filepath << "\n";
    std::cout << "#######################################################################\n";

    std::vector<Instruction> instructions = TACParser::parseFile(filepath);
    if (instructions.empty()) {
        std::cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    std::cout << "Original Instructions Read (" << instructions.size() << "):\n";
    for (size_t i = 0; i < instructions.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << instructions[i].toString() << "\n";
    }

    optimizer.processBlock(instructions);
    Visualizer::printOptimizationLogs(optimizer);
    Visualizer::printDAG(optimizer);

    optimizer.generateOptimizedTAC();
    Visualizer::printSideBySideComparison(optimizer);
    Visualizer::printPerformanceReport(optimizer);

    std::string dotName = "dag_" + testName + ".dot";
    Visualizer::exportGraphviz(optimizer, dotName);
}

void BenchmarkRunner::runAllBenchmarks(DAGOptimizer& optimizer) {
    runBenchmark("TestCase1_README", "samples/sample1_readme.tac", optimizer);
    runBenchmark("TestCase2_Commutative", "samples/sample2_commutative.tac", optimizer);
    runBenchmark("TestCase3_Reassignment", "samples/sample3_reassignment.tac", optimizer);
    runBenchmark("TestCase4_ConstantFolding", "samples/sample4_constant_folding.tac", optimizer);
    runBenchmark("TestCase5_MatrixOffset", "samples/sample5_matrix_offset.tac", optimizer);
}
