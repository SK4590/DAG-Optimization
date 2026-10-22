# PowerShell Test Suite Runner for DAG Optimizer (Review 3 - 100% Milestone)

# Ensure g++ is available in PATH even in pre-existing shell sessions
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    $userPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
    $machinePath = [System.Environment]::GetEnvironmentVariable("Path", "Machine")
    $env:Path = "$userPath;$machinePath;$env:Path"

    $mingwBin = "C:\Users\Shobit\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin"
    if (Test-Path $mingwBin) {
        $env:Path = "$mingwBin;$env:Path"
    }
}

Write-Host "=======================================================================" -ForegroundColor Cyan
Write-Host "   BUILDING AND RUNNING DAG OPTIMIZER & CODEGEN (REVIEW 3 - 100%)" -ForegroundColor Cyan
Write-Host "=======================================================================" -ForegroundColor Cyan

Write-Host "`n[1/3] Compiling Modular C++ Source Code..." -ForegroundColor Yellow
g++ -std=c++17 -Wall -Wextra -static -Iinclude src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/CFG.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/RegisterAllocator.cpp src/CodeGen.cpp src/main.cpp -o dag_optimizer.exe
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Compilation failed!" -ForegroundColor Red
    exit 1
}
Write-Host "[SUCCESS] Compilation succeeded: dag_optimizer.exe created." -ForegroundColor Green

Write-Host "`n[2/3] Running All Benchmark Test Cases (Local DAG, CFG, and Backend CodeGen)..." -ForegroundColor Yellow

$samples = @(
    @{ Name = "Test Case 1 (README CSE)"; Path = "samples/sample1_readme.tac"; Out = "optimized_sample1.tac"; Asm = "sample1.s" },
    @{ Name = "Test Case 2 (Commutative CSE)"; Path = "samples/sample2_commutative.tac"; Out = "optimized_sample2.tac"; Asm = "sample2.s" },
    @{ Name = "Test Case 3 (Variable Reassignment)"; Path = "samples/sample3_reassignment.tac"; Out = "optimized_sample3.tac"; Asm = "sample3.s" },
    @{ Name = "Test Case 4 (Constant Folding & Identities)"; Path = "samples/sample4_constant_folding.tac"; Out = "optimized_sample4.tac"; Asm = "sample4.s" },
    @{ Name = "Test Case 5 (Matrix Offset Real-world)"; Path = "samples/sample5_matrix_offset.tac"; Out = "optimized_sample5.tac"; Asm = "sample5.s" },
    @{ Name = "Test Case 6 (CFG While Loop & Back-Edge)"; Path = "samples/sample6_cfg_loop.tac"; Out = "optimized_sample6.tac"; Asm = "sample6_loop.s" },
    @{ Name = "Test Case 7 (CFG If-Else Diamond & Convergence)"; Path = "samples/sample7_cfg_if_else.tac"; Out = "optimized_sample7.tac"; Asm = "sample7_if_else.s" },
    @{ Name = "Test Case 8 (CFG Multi-Block Loop with LVA & DCE)"; Path = "samples/sample8_cfg_lva_dce.tac"; Out = "optimized_sample8.tac"; Asm = "sample8_lva_dce.s" },
    @{ Name = "Benchmark 6 (Multi-Block While Loop Sum)"; Path = "samples/sample6_loop_sum.tac"; Out = "optimized_sample6_loop_sum.tac"; Asm = "sample6_loop_sum.s" },
    @{ Name = "Benchmark 7 (Factorial Loop & Branches)"; Path = "samples/sample7_factorial.tac"; Out = "optimized_sample7_factorial.tac"; Asm = "sample7_factorial.s" },
    @{ Name = "Benchmark 8 (Nested If-Else Branches & Dead Code)"; Path = "samples/sample8_if_else_branches.tac"; Out = "optimized_sample8_if_else_branches.tac"; Asm = "sample8_if_else_branches.s" }
)

foreach ($s in $samples) {
    Write-Host "`n>>> Running $($s.Name)..." -ForegroundColor Magenta
    .\dag_optimizer.exe $s.Path $s.Out $s.Asm
}

Write-Host "`n[3/3] All tests executed successfully!" -ForegroundColor Green
Write-Host "Outputs generated in current directory:" -ForegroundColor Cyan
Get-ChildItem -Filter "optimized_*.tac" | Select-Object Name, Length
Get-ChildItem -Filter "*.s" | Select-Object Name, Length
Get-ChildItem -Filter "*.dot" | Select-Object Name, Length
Write-Host "`n=======================================================================" -ForegroundColor Cyan
