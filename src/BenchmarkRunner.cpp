#if __has_include("BenchmarkRunner.h")
    #include "BenchmarkRunner.h"
    #include "TACParser.h"
    #include "Visualizer.h"
    #include "CFG.h"
#elif __has_include("include/BenchmarkRunner.h")
    #include "include/BenchmarkRunner.h"
    #include "include/TACParser.h"
    #include "include/Visualizer.h"
    #include "include/CFG.h"
#else
    #include "../include/BenchmarkRunner.h"
    #include "../include/TACParser.h"
    #include "../include/Visualizer.h"
    #include "../include/CFG.h"
#endif

#include <iostream>
#include <fstream>

using namespace std;

void BenchmarkRunner::runBenchmark(const string& testName, const string& filepath, DAGOptimizer& optimizer) {
    cout << "\n#######################################################################\n";
    cout << " RUNNING BENCHMARK: " << testName << " (" << filepath << ")\n";
    cout << "#######################################################################\n";

    vector<Instruction> instructions = TACParser::parseFile(filepath);
    if (instructions.empty()) {
        cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    optimizer.processBlock(instructions);
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

void BenchmarkRunner::runCFGBenchmark(const string& testName, const string& filepath) {
    cout << "\n#######################################################################\n";
    cout << " RUNNING CFG BENCHMARK: " << testName << " (" << filepath << ")\n";
    cout << "#######################################################################\n";

    vector<Instruction> instructions = TACParser::parseFile(filepath);
    if (instructions.empty()) {
        cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    CFG initialCfg;
    initialCfg.construct(instructions);

    Visualizer::printCFG(initialCfg);

    CFG optimizedCfg = initialCfg;
    optimizedCfg.optimizeLocalDAGs();
    int deadEliminated = optimizedCfg.runDeadCodeElimination();

    Visualizer::printMultiBlockComparison(initialCfg, optimizedCfg, deadEliminated);

    string finalDot = "cfg_" + testName + ".dot";
    Visualizer::exportCFG(optimizedCfg, finalDot);

    // Output optimized TAC file
    string outTacPath = "optimized_" + testName + ".tac";
    ofstream outFile(outTacPath);
    if (outFile.is_open()) {
        for (const auto& line : optimizedCfg.getTACLines()) {
            outFile << line << "\n";
        }
        outFile.close();
        cout << "[SUCCESS] Wrote optimized TAC to: " << outTacPath << "\n";
    }
}

void BenchmarkRunner::runAllCFGBenchmarks() {
    runCFGBenchmark("TestCase6_CFG_Loop", "samples/sample6_cfg_loop.tac");
    runCFGBenchmark("TestCase7_CFG_IfElse", "samples/sample7_cfg_if_else.tac");
    runCFGBenchmark("TestCase8_CFG_LVA_DCE", "samples/sample8_cfg_lva_dce.tac");
}

void BenchmarkRunner::runCompleteSuite(DAGOptimizer& optimizer) {
    cout << "\n=======================================================================\n";
    cout << "     EXECUTING COMPLETE BENCHMARK SUITE (LOCAL DAG & MULTI-BLOCK CFG)  \n";
    cout << "=======================================================================\n";
    runAllBenchmarks(optimizer);
    runAllCFGBenchmarks();
}
