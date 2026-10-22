#if __has_include("BenchmarkRunner.h")
    #include "BenchmarkRunner.h"
    #include "TACParser.h"
    #include "Visualizer.h"
    #include "CFG.h"
    #include "CodeGen.h"
    #include "RegisterAllocator.h"
#elif __has_include("include/BenchmarkRunner.h")
    #include "include/BenchmarkRunner.h"
    #include "include/TACParser.h"
    #include "include/Visualizer.h"
    #include "include/CFG.h"
    #include "include/CodeGen.h"
    #include "include/RegisterAllocator.h"
#else
    #include "../include/BenchmarkRunner.h"
    #include "../include/TACParser.h"
    #include "../include/Visualizer.h"
    #include "../include/CFG.h"
    #include "../include/CodeGen.h"
    #include "../include/RegisterAllocator.h"
#endif

#include <iostream>
#include <fstream>

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

void BenchmarkRunner::runCFGBenchmark(const string& testName, const string& filepath) {
    cout << "\n#######################################################################\n";
    cout << " RUNNING CFG BENCHMARK: " << testName << "\n";
    cout << " File: " << filepath << "\n";
    cout << "#######################################################################\n";

    vector<Instruction> instructions = TACParser::parseFile(filepath);
    if (instructions.empty()) {
        cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    CFG initialCfg;
    initialCfg.construct(instructions);

    cout << "\n>>> Phase 1: CFG Leader Identification & Block Partitioning\n";
    Visualizer::printCFG(initialCfg);
    string initialDot = "cfg_" + testName + "_initial.dot";
    Visualizer::exportCFG(initialCfg, initialDot);

    cout << "\n>>> Phase 2: Live Variable Analysis on Initial CFG\n";
    Visualizer::printLVAReport(initialCfg);

    // Multi-Block Optimization Pipeline
    CFG optimizedCfg = initialCfg;
    cout << "\n>>> Phase 3: Multi-Block Local DAG Optimization Pass\n";
    optimizedCfg.optimizeLocalDAGs();

    cout << "\n>>> Phase 4: Global LVA & Global Dead Code Elimination (DCE)\n";
    int deadEliminated = optimizedCfg.runDeadCodeElimination();

    Visualizer::printCFG(optimizedCfg);
    Visualizer::printLVAReport(optimizedCfg);
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

void BenchmarkRunner::runCodeGenBenchmark(const string& testName, const string& filepath) {
    cout << "\n#######################################################################\n";
    cout << " RUNNING TARGET CODEGEN BENCHMARK: " << testName << "\n";
    cout << " Input File: " << filepath << "\n";
    cout << "#######################################################################\n";

    vector<Instruction> origInstructions = TACParser::parseFile(filepath);
    if (origInstructions.empty()) {
        cerr << "[WARNING] No valid TAC instructions found in " << filepath << "\n";
        return;
    }

    // 1. Construct CFG and run multi-block optimizations
    CFG cfg;
    cfg.construct(origInstructions);
    cfg.optimizeLocalDAGs();
    cfg.runDeadCodeElimination();
    vector<Instruction> optInstructions = cfg.getAllInstructions();

    // 2. Generate MIPS Target Machine Assembly
    CodeGen codeGen;
    string asmContent = codeGen.generateAssembly(optInstructions);
    const auto& asmMetrics = codeGen.getMetrics();
    int frameSize = codeGen.getAllocator().getStackFrameSize();

    // 3. Display Side-by-Side 3-Way Metrics
    Visualizer::printThreeWayComparison(origInstructions, optInstructions, 
                                        codeGen.getAssemblyLines(), 
                                        frameSize, asmMetrics.totalSpills);

    // 4. Write optimized TAC and Assembly files
    string outTacPath = "optimized_" + testName + ".tac";
    ofstream outTac(outTacPath);
    if (outTac.is_open()) {
        for (const auto& line : cfg.getTACLines()) {
            outTac << line << "\n";
        }
        outTac.close();
        cout << "[SUCCESS] Wrote optimized TAC to: " << outTacPath << "\n";
    }

    string outAsmPath = testName + ".s";
    ofstream outAsm(outAsmPath);
    if (outAsm.is_open()) {
        outAsm << asmContent;
        outAsm.close();
        cout << "[SUCCESS] Generated Target MIPS Assembly: " << outAsmPath << "\n";
    }
}

void BenchmarkRunner::runAllCodeGenBenchmarks() {
    runCodeGenBenchmark("TestCase6_LoopSum", "samples/sample6_loop_sum.tac");
    runCodeGenBenchmark("TestCase7_Factorial", "samples/sample7_factorial.tac");
    runCodeGenBenchmark("TestCase8_IfElseBranches", "samples/sample8_if_else_branches.tac");
}

void BenchmarkRunner::runCompleteSuite(DAGOptimizer& optimizer) {
    cout << "\n=======================================================================\n";
    cout << "       EXECUTING COMPLETE BENCHMARK SUITE (REVIEWS 1, 2 & 3 - 100%)    \n";
    cout << "=======================================================================\n";
    runAllBenchmarks(optimizer);
    runAllCFGBenchmarks();
    runAllCodeGenBenchmarks();
}


