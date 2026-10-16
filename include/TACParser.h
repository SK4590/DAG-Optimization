#ifndef TACPARSER_H
#define TACPARSER_H

#include <string>
#include <vector>
#include "Instruction.h"

using namespace std;

class TACParser {
public:
    static bool parseLine(const string& raw, Instruction& outInst);
    static vector<Instruction> parseFile(const string& filename);
    static vector<Instruction> parseString(const string& content);
};

#endif // TACPARSER_H
