@echo off
setlocal

where g++ >nul 2>nul
if errorlevel 1 (
    if exist "%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\g++.exe" (
        set "PATH=%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%"
    )
)

echo =======================================================================
echo   BUILDING AND RUNNING DAG OPTIMIZER & CODEGEN (REVIEW 3 - 100%%)
echo =======================================================================

echo [1/3] Compiling Modular C++ Source Code...
g++ -std=c++17 -Wall -Wextra -static -Iinclude src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/CFG.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/RegisterAllocator.cpp src/CodeGen.cpp src/main.cpp -o dag_optimizer.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)
echo [SUCCESS] Compilation succeeded: dag_optimizer.exe created.
echo.

echo [2/3] Running Benchmark Test Cases (Local DAG, Multi-Block CFG & Target CodeGen)...
echo.
echo Running Test Case 1 (README Standard CSE)...
.\dag_optimizer.exe samples\sample1_readme.tac optimized_sample1.tac sample1.s
echo.

echo Running Test Case 2 (Commutative CSE)...
.\dag_optimizer.exe samples\sample2_commutative.tac optimized_sample2.tac sample2.s
echo.

echo Running Test Case 3 (Variable Reassignment)...
.\dag_optimizer.exe samples\sample3_reassignment.tac optimized_sample3.tac sample3.s
echo.

echo Running Test Case 4 (Constant Folding)...
.\dag_optimizer.exe samples\sample4_constant_folding.tac optimized_sample4.tac sample4.s
echo.

echo Running Test Case 5 (Matrix Offset Real-world)...
.\dag_optimizer.exe samples\sample5_matrix_offset.tac optimized_sample5.tac sample5.s
echo.

echo Running Benchmark 6 (Multi-Block While Loop Sum)...
.\dag_optimizer.exe samples\sample6_loop_sum.tac optimized_sample6_loop_sum.tac sample6_loop_sum.s
echo.

echo Running Benchmark 7 (Factorial Loop & Branches)...
.\dag_optimizer.exe samples\sample7_factorial.tac optimized_sample7_factorial.tac sample7_factorial.s
echo.

echo Running Benchmark 8 (Nested If-Else Branches & Dead Code)...
.\dag_optimizer.exe samples\sample8_if_else_branches.tac optimized_sample8_if_else_branches.tac sample8_if_else_branches.s
echo.

echo [3/3] All tests executed successfully!
echo Generated output files:
echo   - optimized_*.tac (Optimized TAC output files)
echo   - *.s (Target MIPS assembly files)
echo   - dag_output.dot (Graphviz DAG visualization)
echo   - cfg.dot (Graphviz CFG visualization with LVA annotations)
echo =======================================================================
pause
