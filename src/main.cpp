#if __has_include("Instruction.h")
    #include "Instruction.h"
    #include "DAGNode.h"
    #include "Utils.h"
    #include "TACParser.h"
    #include "DAGOptimizer.h"
    #include "CFG.h"
    #include "Visualizer.h"
    #include "BenchmarkRunner.h"
    #include "CodeGen.h"
    #include "RegisterAllocator.h"
#elif __has_include("include/Instruction.h")
    #include "include/Instruction.h"
    #include "include/DAGNode.h"
    #include "include/Utils.h"
    #include "include/TACParser.h"
    #include "include/DAGOptimizer.h"
    #include "include/CFG.h"
    #include "include/Visualizer.h"
    #include "include/BenchmarkRunner.h"
    #include "include/CodeGen.h"
    #include "include/RegisterAllocator.h"
#else
    #include "../include/Instruction.h"
    #include "../include/DAGNode.h"
    #include "../include/Utils.h"
    #include "../include/TACParser.h"
    #include "../include/DAGOptimizer.h"
    #include "../include/CFG.h"
    #include "../include/Visualizer.h"
    #include "../include/BenchmarkRunner.h"
    #include "../include/CodeGen.h"
    #include "../include/RegisterAllocator.h"
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
    cout << " 1. Run Complete Benchmark Suite (Local DAG + CFG + Backend CodeGen)\n";
    cout << " 2. Run Review 2 Suite (Local DAG Optimizations 1-5)\n";
    cout << " 3. Run Review 3 CFG Suite (Multi-Block CFG, LVA & DCE Tests)\n";
    cout << " 4. Run Review 3 Backend Suite (MIPS CodeGen & Register Allocation)\n";
    cout << " 5. Run Benchmark 6: While Loop Sum (sample6_loop_sum.tac)\n";
    cout << " 6. Run Benchmark 7: Factorial Loop (sample7_factorial.tac)\n";
    cout << " 7. Run Benchmark 8: Nested If-Else Branches (sample8_if_else_branches.tac)\n";
    cout << " 8. Optimize & Compile Custom TAC File to Target MIPS Assembly\n";
    cout << " 9. Interactive TAC Terminal Input with MIPS CodeGen\n";
    cout << " 10. Exit\n";
    cout << "-----------------------------------------------------------------------\n";
    cout << "Enter your choice [1-10]: ";
}

int main(int argc, char* argv[]) {
    DAGOptimizer optimizer;

    // CLI Argument mode:
    // Syntax 1: ./dag_optimizer <input_file> [output_tac_file] [output_asm_file]
    // Syntax 2: ./dag_optimizer -S <input_file> [output_asm_file]
    // Syntax 3: ./dag_optimizer --asm <input_file> [output_asm_file]
    if (argc >= 2) {
        string inputFile;
        string outputFile = "optimized_output.tac";
        string asmFile = "output.s";
        bool asmOnlyFlag = false;

        if (string(argv[1]) == "-S" || string(argv[1]) == "--asm") {
            asmOnlyFlag = true;
            if (argc < 3) {
                cerr << "[ERROR] Expected input file after " << argv[1] << "\n";
                return 1;
            }
            inputFile = argv[2];
            if (argc >= 4) asmFile = argv[3];
            else {
                size_t dotPos = inputFile.find_last_of('.');
                asmFile = (dotPos != string::npos) ? inputFile.substr(0, dotPos) + ".s" : inputFile + ".s";
            }
        } else {
            inputFile = argv[1];
            if (argc >= 3) {
                outputFile = argv[2];
                size_t dotPos = outputFile.find_last_of('.');
                asmFile = (dotPos != string::npos) ? outputFile.substr(0, dotPos) + ".s" : outputFile + ".s";
            }
            if (argc >= 4) {
                asmFile = argv[3];
            }
        }

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

        vector<Instruction> finalOptimizedInsts;

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

            finalOptimizedInsts = optimizedCfg.getAllInstructions();

            // Write output TAC file
            if (!asmOnlyFlag) {
                ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    for (const auto& line : optimizedCfg.getTACLines()) {
                        outFile << line << "\n";
                    }
                    outFile.close();
                    cout << "[SUCCESS] Wrote optimized TAC to: " << outputFile << "\n";
                }
            }
        } else {
            cout << "[INFO] Single basic block detected. Running local DAG optimization...\n";
            optimizer.processBlock(instructions);
            Visualizer::printOptimizationLogs(optimizer);
            Visualizer::printDAG(optimizer);

            vector<string> optimized = optimizer.generateOptimizedTAC();
            Visualizer::printSideBySideComparison(optimizer);
            Visualizer::printPerformanceReport(optimizer);

            finalOptimizedInsts = optimizer.generateOptimizedInstructions();

            // Write output TAC file
            if (!asmOnlyFlag) {
                ofstream outFile(outputFile);
                if (outFile.is_open()) {
                    for (const auto& line : optimized) {
                        outFile << line << "\n";
                    }
                    outFile.close();
                    cout << "[SUCCESS] Wrote optimized TAC to: " << outputFile << "\n";
                }
            }

            Visualizer::exportGraphviz(optimizer, "dag_output.dot");
            Visualizer::exportCFG(initialCfg, "cfg.dot");
        }

        // --- Target Machine Assembly Code Generation ---
        cout << "\n>>> Phase 5: Register Allocation & MIPS Assembly Code Generation\n";
        CodeGen codeGen;
        string asmCode = codeGen.generateAssembly(finalOptimizedInsts);
        int frameSize = codeGen.getAllocator().getStackFrameSize();
        const auto& metrics = codeGen.getMetrics();

        // Print Side-by-Side 3-Way Metrics
        Visualizer::printThreeWayComparison(instructions, finalOptimizedInsts, 
                                            codeGen.getAssemblyLines(), 
                                            frameSize, metrics.totalSpills);

        // Write assembly file
        ofstream asmOut(asmFile);
        if (asmOut.is_open()) {
            asmOut << asmCode;
            asmOut.close();
            cout << "[SUCCESS] Wrote generated MIPS assembly to: " << asmFile << "\n";
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
            BenchmarkRunner::runAllCodeGenBenchmarks();
        } else if (choice == 5) {
            BenchmarkRunner::runCodeGenBenchmark("TestCase6_LoopSum", "samples/sample6_loop_sum.tac");
        } else if (choice == 6) {
            BenchmarkRunner::runCodeGenBenchmark("TestCase7_Factorial", "samples/sample7_factorial.tac");
        } else if (choice == 7) {
            BenchmarkRunner::runCodeGenBenchmark("TestCase8_IfElseBranches", "samples/sample8_if_else_branches.tac");
        } else if (choice == 8) {
            cout << "Enter path to TAC file: ";
            string path;
            cin >> path;
            vector<Instruction> insts = TACParser::parseFile(path);
            if (insts.empty()) {
                cerr << "[ERROR] Could not parse file: " << path << "\n";
            } else {
                BenchmarkRunner::runCodeGenBenchmark("CustomProgram", path);
            }
        } else if (choice == 9) {
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

                vector<Instruction> finalInsts;
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
                    finalInsts = optCfg.getAllInstructions();
                } else {
                    optimizer.processBlock(userInstructions);
                    Visualizer::printOptimizationLogs(optimizer);
                    Visualizer::printDAG(optimizer);
                    optimizer.generateOptimizedTAC();
                    Visualizer::printSideBySideComparison(optimizer);
                    Visualizer::printPerformanceReport(optimizer);
                    Visualizer::exportGraphviz(optimizer, "dag_interactive.dot");
                    finalInsts = optimizer.generateOptimizedInstructions();
                }

                // Generate MIPS Assembly
                CodeGen codeGen;
                string asmContent = codeGen.generateAssembly(finalInsts);
                int frameSize = codeGen.getAllocator().getStackFrameSize();
                const auto& metrics = codeGen.getMetrics();

                Visualizer::printThreeWayComparison(userInstructions, finalInsts, 
                                                    codeGen.getAssemblyLines(), 
                                                    frameSize, metrics.totalSpills);

                codeGen.writeAssemblyToFile(finalInsts, "interactive_output.s");
                cout << "[SUCCESS] Wrote interactive assembly to: interactive_output.s\n";
            } else {
                cout << "[INFO] No instructions provided.\n";
            }
        } else if (choice == 10) {
            cout << "Exiting DAG Optimizer & Compiler Backend. Thank you!\n";
            break;
        } else {
            cout << "[ERROR] Invalid option. Please select 1-10.\n";
        }
    }

    return 0;
}
