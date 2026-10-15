#ifndef TACPARSER_H
#define TACPARSER_H

#include <string>
#include <vector>
#include "Instruction.h"

class TACParser {
public:
    static bool parseLine(const std::string& raw, Instruction& outInst);
    static std::vector<Instruction> parseFile(const std::string& filename);
    static std::vector<Instruction> parseString(const std::string& content);
};

#endif // TACPARSER_H
