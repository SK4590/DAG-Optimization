#if __has_include("Utils.h")
    #include "Utils.h"
#else
    #include "../include/Utils.h"
#endif

#include <sstream>
#include <cmath>

using namespace std;

namespace Utils {
    string trim(const string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    bool isNumber(const string& s, double& outVal) {
        if (s.empty()) return false;
        try {
            size_t idx = 0;
            outVal = stod(s, &idx);
            return idx == s.length();
        } catch (...) {
            return false;
        }
    }

    string formatNumber(double val) {
        if (floor(val) == val) {
            return to_string((long long)val);
        }
        stringstream ss;
        ss << val;
        return ss.str();
    }

    bool isCommutative(const string& op) {
        return (op == "+" || op == "*" || op == "==" || op == "!=" || 
                op == "&" || op == "|" || op == "^");
    }
}
