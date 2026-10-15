#if __has_include("Instruction.h")
    #include "Instruction.h"
    #include "DAGNode.h"
    #include "Utils.h"
    #include "TACParser.h"
    #include "DAGOptimizer.h"
    #include "Visualizer.h"
    #include "BenchmarkRunner.h"
#else
    #include "../include/Instruction.h"
    #include "../include/DAGNode.h"
    #include "../include/Utils.h"
    #include "../include/TACParser.h"
    #include "../include/DAGOptimizer.h"
    #include "../include/Visualizer.h"
    #include "../include/BenchmarkRunner.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void displayMenu() {
    std::cout << "\n=======================================================================\n";
    std::cout << "     DAG-BASED BASIC BLOCK OPTIMIZER (REVIEW 2 - 75% MILESTONE)        \n";
    std::cout << "=======================================================================\n";
    std::cout << " 1. Run Complete Benchmark Suite (All 5 Built-in Test Cases)\n";
    std::cout << " 2. Run Test Case 1: Standard CSE (from Project README)\n";
    std::cout << " 3. Run Test Case 2: Commutative CSE (a+b == b+a, x*y == y*x)\n";
    std::cout << " 4. Run Test Case 3: Variable Reassignment with Node Kill Semantics\n";
    std::cout << " 5. Run Test Case 4: Constant Folding & Algebraic Identities\n";
    std::cout << " 6. Run Test Case 5: Realistic Matrix Array Offset Calculation\n";
    std::cout << " 7. Optimize Custom TAC File\n";
    std::cout << " 8. Interactive TAC Terminal Input\n";
    std::cout << " 9. Exit\n";
    std::cout << "-----------------------------------------------------------------------\n";
    std::cout << "Enter your choice [1-9]: ";
}

int main(int argc, char* argv[]) {
    DAGOptimizer optimizer;

    // CLI Argument mode: ./dag_optimizer <input_file> [output_file]
    if (argc >= 2) {
        std::string inputFile = argv[1];
        std::string outputFile = (argc >= 3) ? argv[2] : "optimized_output.tac";

        std::cout << "[INFO] Processing TAC file from command line: " << inputFile << "\n";
        std::vector<Instruction> instructions = TACParser::parseFile(inputFile);
        if (instructions.empty()) {
            std::cerr << "[ERROR] Could not parse TAC instructions from " << inputFile << "\n";
            return 1;
        }

        optimizer.processBlock(instructions);
        Visualizer::printOptimizationLogs(optimizer);
        Visualizer::printDAG(optimizer);

        std::vector<std::string> optimized = optimizer.generateOptimizedTAC();
        Visualizer::printSideBySideComparison(optimizer);
        Visualizer::printPerformanceReport(optimizer);

        // Write output TAC file
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            for (const auto& line : optimized) {
                outFile << line << "\n";
            }
            outFile.close();
            std::cout << "[SUCCESS] Wrote optimized TAC to: " << outputFile << "\n";
        }

        Visualizer::exportGraphviz(optimizer, "dag_output.dot");
        return 0;
    }

    // Interactive Menu Mode
    int choice = 0;
    while (true) {
        displayMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string discard;
            std::getline(std::cin, discard);
            std::cout << "[ERROR] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            BenchmarkRunner::runAllBenchmarks(optimizer);
        } else if (choice == 2) {
            BenchmarkRunner::runBenchmark("TestCase1_README", "samples/sample1_readme.tac", optimizer);
        } else if (choice == 3) {
            BenchmarkRunner::runBenchmark("TestCase2_Commutative", "samples/sample2_commutative.tac", optimizer);
        } else if (choice == 4) {
            BenchmarkRunner::runBenchmark("TestCase3_Reassignment", "samples/sample3_reassignment.tac", optimizer);
        } else if (choice == 5) {
            BenchmarkRunner::runBenchmark("TestCase4_ConstantFolding", "samples/sample4_constant_folding.tac", optimizer);
        } else if (choice == 6) {
            BenchmarkRunner::runBenchmark("TestCase5_MatrixOffset", "samples/sample5_matrix_offset.tac", optimizer);
        } else if (choice == 7) {
            std::cout << "Enter path to TAC file: ";
            std::string path;
            std::cin >> path;
            BenchmarkRunner::runBenchmark("CustomFile", path, optimizer);
        } else if (choice == 8) {
            std::cout << "\nEnter TAC instructions line by line (Type 'END' on a new line when done):\n";
            std::string line;
            std::cin.ignore();
            std::vector<Instruction> userInstructions;
            while (std::getline(std::cin, line)) {
                if (Utils::trim(line) == "END") break;
                Instruction inst;
                if (TACParser::parseLine(line, inst)) {
                    userInstructions.push_back(inst);
                } else if (!Utils::trim(line).empty()) {
                    std::cout << "  [Warning: skipped invalid TAC line: " << line << "]\n";
                }
            }

            if (!userInstructions.empty()) {
                optimizer.processBlock(userInstructions);
                Visualizer::printOptimizationLogs(optimizer);
                Visualizer::printDAG(optimizer);
                optimizer.generateOptimizedTAC();
                Visualizer::printSideBySideComparison(optimizer);
                Visualizer::printPerformanceReport(optimizer);
                Visualizer::exportGraphviz(optimizer, "dag_interactive.dot");
            } else {
                std::cout << "[INFO] No instructions provided.\n";
            }
        } else if (choice == 9) {
            std::cout << "Exiting DAG Optimizer. Thank you!\n";
            break;
        } else {
            std::cout << "[ERROR] Invalid option. Please select 1-9.\n";
        }
    }

    return 0;
}
