#if __has_include("TACParser.h")
    #include "TACParser.h"
    #include "Utils.h"
#else
    #include "../include/TACParser.h"
    #include "../include/Utils.h"
#endif

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

bool TACParser::parseLine(const string& raw, Instruction& outInst) {
    string line = Utils::trim(raw);
    if (line.empty() || line[0] == '#' || (line.size() >= 2 && line[0] == '/' && line[1] == '/')) {
        return false; // Skip comments and empty lines
    }

    outInst.rawLine = line;

    // Check for assignment operator '='
    size_t eqPos = line.find('=');
    if (eqPos == string::npos) {
        outInst.type = InstructionType::UNKNOWN;
        return false;
    }

    string leftSide = Utils::trim(line.substr(0, eqPos));
    string rightSide = Utils::trim(line.substr(eqPos + 1));
    outInst.result = leftSide;

    // Tokenize right side
    stringstream ss(rightSide);
    vector<string> tokens;
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return false;
    }

    double numVal;
    if (tokens.size() == 1) {
        outInst.arg1 = tokens[0];
        if (Utils::isNumber(tokens[0], numVal)) {
            outInst.type = InstructionType::CONSTANT;
        } else {
            outInst.type = InstructionType::COPY;
        }
    } else if (tokens.size() == 2) {
        // Unary expression: op arg1 (e.g., - x, ! b, ~ a)
        outInst.type = InstructionType::UNARY;
        outInst.op = tokens[0];
        outInst.arg1 = tokens[1];
    } else if (tokens.size() == 3) {
        // Binary expression: arg1 op arg2
        outInst.type = InstructionType::BINARY;
        outInst.arg1 = tokens[0];
        outInst.op = tokens[1];
        outInst.arg2 = tokens[2];
    } else {
        outInst.type = InstructionType::UNKNOWN;
        return false;
    }

    return true;
}

vector<Instruction> TACParser::parseFile(const string& filename) {
    vector<Instruction> instructions;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Unable to open TAC input file: " << filename << "\n";
        return instructions;
    }
    string line;
    while (getline(file, line)) {
        Instruction inst;
        if (parseLine(line, inst)) {
            instructions.push_back(inst);
        }
    }
    return instructions;
}

vector<Instruction> TACParser::parseString(const string& content) {
    vector<Instruction> instructions;
    stringstream ss(content);
    string line;
    while (getline(ss, line)) {
        Instruction inst;
        if (parseLine(line, inst)) {
            instructions.push_back(inst);
        }
    }
    return instructions;
}
