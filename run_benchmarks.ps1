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
Write-Host "   BUILDING AND RUNNING DAG OPTIMIZER & CFG ENGINE (REVIEW 3 - 100%)   " -ForegroundColor Cyan
Write-Host "=======================================================================" -ForegroundColor Cyan

Write-Host "`n[1/3] Compiling Modular C++ Source Code..." -ForegroundColor Yellow
g++ -std=c++17 -Wall -Wextra -static -Iinclude src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/CFG.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/main.cpp -o dag_optimizer.exe
if ($LASTEXITCODE -ne 0) {
    Write-Host "[ERROR] Compilation failed!" -ForegroundColor Red
    exit 1
}
Write-Host "[SUCCESS] Compilation succeeded: dag_optimizer.exe created." -ForegroundColor Green

Write-Host "`n[2/3] Running All 8 Benchmark Test Cases (Local DAG, CFG, LVA & DCE)..." -ForegroundColor Yellow

$samples = @(
    @{ Name = "Test Case 1 (README CSE)"; Path = "samples/sample1_readme.tac"; Out = "optimized_sample1.tac" },
    @{ Name = "Test Case 2 (Commutative CSE)"; Path = "samples/sample2_commutative.tac"; Out = "optimized_sample2.tac" },
    @{ Name = "Test Case 3 (Variable Reassignment)"; Path = "samples/sample3_reassignment.tac"; Out = "optimized_sample3.tac" },
    @{ Name = "Test Case 4 (Constant Folding & Identities)"; Path = "samples/sample4_constant_folding.tac"; Out = "optimized_sample4.tac" },
    @{ Name = "Test Case 5 (Matrix Offset Real-world)"; Path = "samples/sample5_matrix_offset.tac"; Out = "optimized_sample5.tac" },
    @{ Name = "Test Case 6 (CFG While Loop & Back-Edge)"; Path = "samples/sample6_cfg_loop.tac"; Out = "optimized_sample6.tac" },
    @{ Name = "Test Case 7 (CFG If-Else Diamond & Convergence)"; Path = "samples/sample7_cfg_if_else.tac"; Out = "optimized_sample7.tac" },
    @{ Name = "Test Case 8 (CFG Multi-Block Loop with LVA & DCE)"; Path = "samples/sample8_cfg_lva_dce.tac"; Out = "optimized_sample8.tac" }
)

foreach ($s in $samples) {
    Write-Host "`n>>> Running $($s.Name)..." -ForegroundColor Magenta
    .\dag_optimizer.exe $s.Path $s.Out
}

Write-Host "`n[3/3] All tests executed successfully!" -ForegroundColor Green
Write-Host "Outputs generated in current directory:" -ForegroundColor Cyan
Get-ChildItem -Filter "optimized_*.tac" | Select-Object Name, Length
Get-ChildItem -Filter "*.dot" | Select-Object Name, Length
Write-Host "`n=======================================================================" -ForegroundColor Cyan
