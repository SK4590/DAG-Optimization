#if __has_include("Instruction.h")
    #include "Instruction.h"
    #include "DAGNode.h"
    #include "Utils.h"
    #include "TACParser.h"
    #include "DAGOptimizer.h"
    #include "CFG.h"
    #include "Visualizer.h"
    #include "BenchmarkRunner.h"
#else
    #include "../include/Instruction.h"
    #include "../include/DAGNode.h"
    #include "../include/Utils.h"
    #include "../include/TACParser.h"
    #include "../include/DAGOptimizer.h"
    #include "../include/CFG.h"
    #include "../include/Visualizer.h"
    #include "../include/BenchmarkRunner.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void displayMenu() {
    cout << "\n=======================================================================\n";
    cout << "     DAG-BASED OPTIMIZATION & CFG ENGINE (REVIEW 3 - 100% COMPLETE)    \n";
    cout << "=======================================================================\n";
    cout << " 1. Run Complete Benchmark Suite (All 8 Tests: Review 2 + Review 3)\n";
    cout << " 2. Run Review 2 Suite (Local DAG Optimizations 1-5)\n";
    cout << " 3. Run Review 3 Suite (Multi-Block CFG, LVA & DCE Tests 6-8)\n";
    cout << " 4. Run Test Case 6: While Loop with Conditional Branch & Back-Edge\n";
    cout << " 5. Run Test Case 7: If-Else Diamond with Branching & Fallthrough\n";
    cout << " 6. Run Test Case 8: Multi-Block Loop with Transitive LVA & DCE\n";
    cout << " 7. Optimize Custom TAC File (Auto-detect Single-Block vs Multi-Block)\n";
    cout << " 8. Interactive TAC Terminal Input\n";
    cout << " 9. Exit\n";
    cout << "-----------------------------------------------------------------------\n";
    cout << "Enter your choice [1-9]: ";
}

int main(int argc, char* argv[]) {
    DAGOptimizer optimizer;

    // CLI Argument mode: ./dag_optimizer <input_file> [output_file]
    if (argc >= 2) {
        string inputFile = argv[1];
        string outputFile = (argc >= 3) ? argv[2] : "optimized_output.tac";

        cout << "[INFO] Processing TAC file from command line: " << inputFile << "\n";
        vector<Instruction> instructions = TACParser::parseFile(inputFile);
        if (instructions.empty()) {
            cerr << "[ERROR] Could not parse TAC instructions from " << inputFile << "\n";
            return 1;
        }

        bool hasControlFlow = false;
        for (const auto& inst : instructions) {
            if (inst.isControlFlow()) {
                hasControlFlow = true;
                break;
            }
        }

        CFG initialCfg;
        initialCfg.construct(instructions);

        if (hasControlFlow || initialCfg.getBlockCount() > 1) {
            cout << "[INFO] Multi-Block Control Flow Graph detected (" 
                 << initialCfg.getBlockCount() << " basic blocks).\n";
            
            cout << "\n>>> Phase 1: CFG Topology & Leader Identification\n";
            Visualizer::printCFG(initialCfg);

            cout << "\n>>> Phase 2: Live Variable Analysis on Initial CFG\n";
            Visualizer::printLVAReport(initialCfg);

            CFG optimizedCfg = initialCfg;
            cout << "\n>>> Phase 3: Multi-Block Local DAG Optimization Pass\n";
            optimizedCfg.optimizeLocalDAGs();

            cout << "\n>>> Phase 4: Global Live Variable Analysis & Dead Code Elimination (DCE)\n";
            int deadEliminated = optimizedCfg.runDeadCodeElimination();

            Visualizer::printCFG(optimizedCfg);
            Visualizer::printLVAReport(optimizedCfg);
            Visualizer::printMultiBlockComparison(initialCfg, optimizedCfg, deadEliminated);

            // Export Graphviz CFG DOT file
            Visualizer::exportCFG(optimizedCfg, "cfg.dot");

            // Write output TAC file
            ofstream outFile(outputFile);
            if (outFile.is_open()) {
                for (const auto& line : optimizedCfg.getTACLines()) {
                    outFile << line << "\n";
                }
                outFile.close();
                cout << "[SUCCESS] Wrote optimized TAC to: " << outputFile << "\n";
            }
        } else {
            cout << "[INFO] Single basic block detected. Running local DAG optimization...\n";
            optimizer.processBlock(instructions);
            Visualizer::printOptimizationLogs(optimizer);
            Visualizer::printDAG(optimizer);

            vector<string> optimized = optimizer.generateOptimizedTAC();
            Visualizer::printSideBySideComparison(optimizer);
            Visualizer::printPerformanceReport(optimizer);

            // Write output TAC file
            ofstream outFile(outputFile);
            if (outFile.is_open()) {
                for (const auto& line : optimized) {
                    outFile << line << "\n";
                }
                outFile.close();
                cout << "[SUCCESS] Wrote optimized TAC to: " << outputFile << "\n";
            }

            Visualizer::exportGraphviz(optimizer, "dag_output.dot");
            Visualizer::exportCFG(initialCfg, "cfg.dot");
        }
        return 0;
    }

    // Interactive Menu Mode
    int choice = 0;
    while (true) {
        displayMenu();
        if (!(cin >> choice)) {
            cin.clear();
            string discard;
            getline(cin, discard);
            cout << "[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            BenchmarkRunner::runCompleteSuite(optimizer);
        } else if (choice == 2) {
            BenchmarkRunner::runAllBenchmarks(optimizer);
        } else if (choice == 3) {
            BenchmarkRunner::runAllCFGBenchmarks();
        } else if (choice == 4) {
            BenchmarkRunner::runCFGBenchmark("TestCase6_CFG_Loop", "samples/sample6_cfg_loop.tac");
        } else if (choice == 5) {
            BenchmarkRunner::runCFGBenchmark("TestCase7_CFG_IfElse", "samples/sample7_cfg_if_else.tac");
        } else if (choice == 6) {
            BenchmarkRunner::runCFGBenchmark("TestCase8_CFG_LVA_DCE", "samples/sample8_cfg_lva_dce.tac");
        } else if (choice == 7) {
            cout << "Enter path to TAC file: ";
            string path;
            cin >> path;
            vector<Instruction> insts = TACParser::parseFile(path);
            if (insts.empty()) {
                cerr << "[ERROR] Could not parse file: " << path << "\n";
            } else {
                bool hasCF = false;
                for (const auto& i : insts) {
                    if (i.isControlFlow()) { hasCF = true; break; }
                }
                if (hasCF) {
                    BenchmarkRunner::runCFGBenchmark("CustomCFG", path);
                } else {
                    BenchmarkRunner::runBenchmark("CustomSingleBlock", path, optimizer);
                }
            }
        } else if (choice == 8) {
            cout << "\nEnter TAC instructions line by line (Type 'END' on a new line when done):\n";
            string line;
            cin.ignore();
            vector<Instruction> userInstructions;
            while (getline(cin, line)) {
                if (Utils::trim(line) == "END") break;
                Instruction inst;
                if (TACParser::parseLine(line, inst)) {
                    userInstructions.push_back(inst);
                } else if (!Utils::trim(line).empty()) {
                    cout << "  [Warning: skipped invalid TAC line: " << line << "]\n";
                }
            }

            if (!userInstructions.empty()) {
                bool hasCF = false;
                for (const auto& i : userInstructions) {
                    if (i.isControlFlow()) { hasCF = true; break; }
                }
                if (hasCF) {
                    CFG cfg;
                    cfg.construct(userInstructions);
                    cout << "\n>>> Initial CFG Partitioning:\n";
                    Visualizer::printCFG(cfg);
                    Visualizer::printLVAReport(cfg);

                    CFG optCfg = cfg;
                    optCfg.optimizeLocalDAGs();
                    int deadElim = optCfg.runDeadCodeElimination();

                    cout << "\n>>> Post-Optimization CFG:\n";
                    Visualizer::printCFG(optCfg);
                    Visualizer::printLVAReport(optCfg);
                    Visualizer::printMultiBlockComparison(cfg, optCfg, deadElim);
                    Visualizer::exportCFG(optCfg, "cfg_interactive.dot");
                } else {
                    optimizer.processBlock(userInstructions);
                    Visualizer::printOptimizationLogs(optimizer);
                    Visualizer::printDAG(optimizer);
                    optimizer.generateOptimizedTAC();
                    Visualizer::printSideBySideComparison(optimizer);
                    Visualizer::printPerformanceReport(optimizer);
                    Visualizer::exportGraphviz(optimizer, "dag_interactive.dot");
                }
            } else {
                cout << "[INFO] No instructions provided.\n";
            }
        } else if (choice == 9) {
            cout << "Exiting DAG Optimizer & CFG Engine. Thank you!\n";
            break;
        } else {
            cout << "[ERROR] Invalid option. Please select 1-9.\n";
        }
    }

    return 0;
}

