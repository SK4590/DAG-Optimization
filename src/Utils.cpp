#if __has_include("Utils.h")
    #include "Utils.h"
#else
    #include "../include/Utils.h"
#endif

#include <sstream>
#include <cmath>

namespace Utils {
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    bool isNumber(const std::string& s, double& outVal) {
        if (s.empty()) return false;
        try {
            size_t idx = 0;
            outVal = std::stod(s, &idx);
            return idx == s.length();
        } catch (...) {
            return false;
        }
    }

    std::string formatNumber(double val) {
        if (std::floor(val) == val) {
            return std::to_string((long long)val);
        }
        std::stringstream ss;
        ss << val;
        return ss.str();
    }

    bool isCommutative(const std::string& op) {
        return (op == "+" || op == "*" || op == "==" || op == "!=" || 
                op == "&" || op == "|" || op == "^");
    }
}
