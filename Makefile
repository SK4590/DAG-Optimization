CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -static -Iinclude
SRC = src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/CFG.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/main.cpp
TARGET = dag_optimizer.exe

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	.\$(TARGET) samples/sample1_readme.tac optimized_sample1.tac
	.\$(TARGET) samples/sample2_commutative.tac optimized_sample2.tac
	.\$(TARGET) samples/sample3_reassignment.tac optimized_sample3.tac
	.\$(TARGET) samples/sample4_constant_folding.tac optimized_sample4.tac
	.\$(TARGET) samples/sample5_matrix_offset.tac optimized_sample5.tac
	.\$(TARGET) samples/sample6_cfg_loop.tac optimized_sample6.tac
	.\$(TARGET) samples/sample7_cfg_if_else.tac optimized_sample7.tac
	.\$(TARGET) samples/sample8_cfg_lva_dce.tac optimized_sample8.tac

clean:
	powershell -Command "Remove-Item -Force -ErrorAction SilentlyContinue $(TARGET), *.dot, optimized_*.tac"

.PHONY: all test clean
