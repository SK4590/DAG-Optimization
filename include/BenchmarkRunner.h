#ifndef BENCHMARKRUNNER_H
#define BENCHMARKRUNNER_H

#include <string>
#include "DAGOptimizer.h"

class BenchmarkRunner {
public:
    static void runBenchmark(const std::string& testName, const std::string& filepath, DAGOptimizer& optimizer);
    static void runAllBenchmarks(DAGOptimizer& optimizer);
};

#endif // BENCHMARKRUNNER_H
