// ============================================================================
// Single-Translation-Unit Wrapper for Modular DAG Optimizer
//
// Recommended Modular Build Command:
//   g++ -std=c++17 -Iinclude src/*.cpp -o dag_optimizer.exe
//
// Legacy Single-File Build Command (also supported):
//   g++ -std=c++17 dag_optimizer.cpp -o dag_optimizer.exe
// ============================================================================

#include "include/Instruction.h"
#include "include/DAGNode.h"
#include "include/Utils.h"
#include "include/TACParser.h"
#include "include/DAGOptimizer.h"
#include "include/CFG.h"
#include "include/Visualizer.h"
#include "include/BenchmarkRunner.h"

#include "src/Utils.cpp"
#include "src/TACParser.cpp"
#include "src/DAGOptimizer.cpp"
#include "src/CFG.cpp"
#include "src/Visualizer.cpp"
#include "src/BenchmarkRunner.cpp"
#include "src/main.cpp"
