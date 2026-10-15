CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRC = src/Utils.cpp src/TACParser.cpp src/DAGOptimizer.cpp src/Visualizer.cpp src/BenchmarkRunner.cpp src/main.cpp
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

clean:
	del /Q $(TARGET) *.dot optimized_*.tac 2>nul || rm -f $(TARGET) *.dot optimized_*.tac

.PHONY: all test clean
