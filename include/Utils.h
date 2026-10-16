#ifndef UTILS_H
#define UTILS_H

#include <string>

using namespace std;

namespace Utils {
    string trim(const string& s);
    bool isNumber(const string& s, double& outVal);
    string formatNumber(double val);
    bool isCommutative(const string& op);
}

#endif // UTILS_H
