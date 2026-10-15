#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string trim(const std::string& s);
    bool isNumber(const std::string& s, double& outVal);
    std::string formatNumber(double val);
    bool isCommutative(const std::string& op);
}

#endif // UTILS_H
