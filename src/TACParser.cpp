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

    // Strip inline and full-line comments
    size_t hashPos = line.find('#');
    if (hashPos != string::npos) {
        line = line.substr(0, hashPos);
    }
    size_t slashPos = line.find("//");
    if (slashPos != string::npos) {
        line = line.substr(0, slashPos);
    }
    line = Utils::trim(line);
    if (line.empty()) {
        return false;
    }

    outInst = Instruction();
    outInst.rawLine = line;

    // 1. Check for Label definition: e.g. "L1:" or "LOOP:"
    if (line.back() == ':') {
        string lbl = Utils::trim(line.substr(0, line.size() - 1));
        if (!lbl.empty() && lbl.find(' ') == string::npos && lbl.find('\t') == string::npos) {
            outInst.type = InstructionType::LABEL;
            outInst.targetLabel = lbl;
            return true;
        }
    }

    // Tokenize line for keyword checks
    stringstream ss(line);
    vector<string> tokens;
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return false;
    }

    // 2. Unconditional jump: goto <label>
    if (tokens[0] == "goto") {
        if (tokens.size() >= 2) {
            outInst.type = InstructionType::GOTO;
            outInst.targetLabel = tokens[1];
            return true;
        }
        outInst.type = InstructionType::UNKNOWN;
        return false;
    }

    // 3. Conditional branches:
    //    if x < y goto L1
    //    if x goto L1
    //    ifFalse a == b goto L2
    //    ifFalse a goto L2
    if (tokens[0] == "if" || tokens[0] == "ifFalse" || tokens[0] == "iffalse") {
        bool isIfFalse = (tokens[0] == "ifFalse" || tokens[0] == "iffalse");
        outInst.type = isIfFalse ? InstructionType::IF_FALSE_GOTO : InstructionType::IF_GOTO;

        // Find "goto" token index
        size_t gotoIdx = 0;
        for (size_t i = 1; i < tokens.size(); ++i) {
            if (tokens[i] == "goto") {
                gotoIdx = i;
                break;
            }
        }

        if (gotoIdx == 0 || gotoIdx + 1 >= tokens.size()) {
            outInst.type = InstructionType::UNKNOWN;
            return false;
        }

        outInst.targetLabel = tokens[gotoIdx + 1];

        if (gotoIdx == 2) {
            // Unary condition: if x goto L1
            outInst.arg1 = tokens[1];
            outInst.op = "";
            outInst.arg2 = "";
        } else if (gotoIdx == 4) {
            // Binary condition: if x < y goto L1
            outInst.arg1 = tokens[1];
            outInst.op = tokens[2];
            outInst.arg2 = tokens[3];
        } else {
            outInst.type = InstructionType::UNKNOWN;
            return false;
        }
        return true;
    }

    // 4. Standard assignment instructions (containing '=')
    size_t eqPos = line.find('=');
    if (eqPos == string::npos) {
        outInst.type = InstructionType::UNKNOWN;
        return false;
    }

    string leftSide = Utils::trim(line.substr(0, eqPos));
    string rightSide = Utils::trim(line.substr(eqPos + 1));
    outInst.result = leftSide;

    // Tokenize right side
    stringstream rightSS(rightSide);
    vector<string> rightTokens;
    string rTok;
    while (rightSS >> rTok) {
        rightTokens.push_back(rTok);
    }

    if (rightTokens.empty()) {
        return false;
    }

    double numVal;
    if (rightTokens.size() == 1) {
        outInst.arg1 = rightTokens[0];
        if (Utils::isNumber(rightTokens[0], numVal)) {
            outInst.type = InstructionType::CONSTANT;
        } else {
            outInst.type = InstructionType::COPY;
        }
    } else if (rightTokens.size() == 2) {
        // Unary expression: op arg1 (e.g., - x, ! b, ~ a)
        outInst.type = InstructionType::UNARY;
        outInst.op = rightTokens[0];
        outInst.arg1 = rightTokens[1];
    } else if (rightTokens.size() == 3) {
        // Binary expression: arg1 op arg2
        outInst.type = InstructionType::BINARY;
        outInst.arg1 = rightTokens[0];
        outInst.op = rightTokens[1];
        outInst.arg2 = rightTokens[2];
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
