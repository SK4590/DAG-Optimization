#ifndef BENCHMARKRUNNER_H
#define BENCHMARKRUNNER_H

#include <string>
#include "DAGOptimizer.h"

using namespace std;

class BenchmarkRunner {
public:
    static void runBenchmark(const string& testName, const string& filepath, DAGOptimizer& optimizer);
    static void runAllBenchmarks(DAGOptimizer& optimizer);
};

#endif // BENCHMARKRUNNER_H
