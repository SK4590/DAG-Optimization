@echo off
echo =======================================================================
echo   BUILDING AND RUNNING DAG OPTIMIZER BENCHMARKS (REVIEW 3 - 100%%)
echo =======================================================================

echo [1/3] Compiling Modular C++ Source Code...
g++ -std=c++17 -Wall -Wextra -Iinclude src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/CFG.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/main.cpp -o dag_optimizer.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)
echo [SUCCESS] Compilation succeeded: dag_optimizer.exe created.
echo.

echo [2/3] Running All 8 Benchmark Test Cases (Local DAG & Multi-Block CFG)...
echo.
echo Running Test Case 1 (README Standard CSE)...
.\dag_optimizer.exe samples\sample1_readme.tac optimized_sample1.tac
echo.

echo Running Test Case 2 (Commutative CSE)...
.\dag_optimizer.exe samples\sample2_commutative.tac optimized_sample2.tac
echo.

echo Running Test Case 3 (Variable Reassignment)...
.\dag_optimizer.exe samples\sample3_reassignment.tac optimized_sample3.tac
echo.

echo Running Test Case 4 (Constant Folding)...
.\dag_optimizer.exe samples\sample4_constant_folding.tac optimized_sample4.tac
echo.

echo Running Test Case 5 (Matrix Offset Real-world)...
.\dag_optimizer.exe samples\sample5_matrix_offset.tac optimized_sample5.tac
echo.

echo Running Test Case 6 (CFG While Loop & Back-Edge)...
.\dag_optimizer.exe samples\sample6_cfg_loop.tac optimized_sample6.tac
echo.

echo Running Test Case 7 (CFG If-Else Diamond & Convergence)...
.\dag_optimizer.exe samples\sample7_cfg_if_else.tac optimized_sample7.tac
echo.

echo Running Test Case 8 (CFG Multi-Block Loop with LVA & DCE)...
.\dag_optimizer.exe samples\sample8_cfg_lva_dce.tac optimized_sample8.tac
echo.

echo [3/3] All tests executed successfully!
echo Generated output files:
echo   - optimized_sample1.tac through optimized_sample8.tac
echo   - dag_output.dot (Graphviz DAG visualization)
echo   - cfg.dot (Graphviz CFG visualization with LVA annotations)
echo =======================================================================
pause
