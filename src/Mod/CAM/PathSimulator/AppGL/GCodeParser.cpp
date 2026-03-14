#ifdef _MSC_VER
# ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
# endif
#endif

#include "GCodeParser.h"

using namespace MillSim;

static char TokTypes[] = "GTXYZIJKR";

GCodeParser::~GCodeParser()
{
    // Clear the vector
    Operations.clear();
}

bool GCodeParser::Parse(const char* filename)
{
    Operations.clear();
    lastState = {eNop, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    lastTool = -1;

    FILE* fl;
    if ((fl = fopen(filename, "rt")) == nullptr) {
        return false;
    }

    char line[120];

    while (!feof(fl)) {
        if (fgets(line, 120, fl) != NULL) {
            AddLine(line);
        }
    }
    fclose(fl);
    return false;
}

const char* GCodeParser::GetNextToken(const char* ptr, GCToken* token)
{
    float tokval;
    token->letter = '*';
    while (*ptr != 0) {
        char letter = toupper(*ptr);
        ptr++;

        if (letter == ' ') {
            continue;
        }
        if (letter == '(') {
            break;
        }

        if (IsValidToken(letter)) {
            ptr = ParseFloat(ptr, &tokval);
            token->letter = letter;
            token->fval = tokval;
            token->ival = (int)(tokval + 0.5);
            break;
        }
    }
    return ptr;
}

bool GCodeParser::IsValidToken(char tok)
{
    int len = (int)strlen(TokTypes);
    for (int i = 0; i < len; i++) {
        if (tok == TokTypes[i]) {
            return true;
        }
    }
    return false;
}

const char* GCodeParser::ParseFloat(const char* ptr, float* retFloat)
{
    float decPos = 10;
    float sign = 1;
    bool decimalPointFound = false;
    float res = 0;
    while (*ptr != 0) {
        char letter = toupper(*ptr);
        ptr++;

        if (letter == ' ') {
            continue;
        }

        if (letter == '-') {
            sign = -1;
        }
        else if (letter == '.') {
            decimalPointFound = true;
        }
        else if (letter >= '0' && letter <= '9') {
            float digitVal = (float)(letter - '0');
            if (decimalPointFound) {
                res = res + digitVal / decPos;
                decPos *= 10;
            }
            else {
                res = res * 10 + digitVal;
            }
        }
        else {
            ptr--;
            break;
        }
    }
    *retFloat = res * sign;
    return ptr;
}

bool GCodeParser::ParseLine(const char* ptr)
{
    // Truncate at first semicolon (annotations / comment)
    const char* comment = strchr(ptr, ';');
    std::string line;
    if (comment) {
        line = std::string(ptr, comment - ptr);
        ptr = line.c_str();
    }

    GCToken token;
    bool validMotion = false;
    bool exitLoop = false;
    int cmd = 0;
    while (*ptr != 0 && !exitLoop) {
        ptr = GetNextToken(ptr, &token);
        lastLastState = lastState;
        switch (token.letter) {
            case '*':
                exitLoop = true;
                break;

            case 'G':
                cmd = token.ival;
                if (cmd == 0 || cmd == 1) {
                    lastState.cmd = eMoveLiner;
                }
                else if (cmd == 2) {
                    lastState.cmd = eRotateCW;
                }
                else if (cmd == 3) {
                    lastState.cmd = eRotateCCW;
                }
                else if (cmd == 73 || cmd == 81 || cmd == 82 || cmd == 83) {
                    lastState.cmd = eDril;
                    lastState.retract_z = lastState.z;
                }
                else if (cmd == 98 || cmd == 99) {
                    lastState.retract_mode = cmd;
                }
                else if (cmd == 80) {
                    lastState.retract_mode = 0;
                }
                break;

            case 'T':
                lastState.tool = token.ival;
                break;

            case 'X':
                lastState.x = token.fval;
                validMotion = true;
                break;

            case 'Y':
                lastState.y = token.fval;
                validMotion = true;
                break;

            case 'Z':
                lastState.z = token.fval;
                validMotion = true;
                break;

            case 'I':
                lastState.i = token.fval;
                break;

            case 'J':
                lastState.j = token.fval;
                break;

            case 'K':
                lastState.k = token.fval;
                break;

            case 'R':
                lastState.r = token.fval;
                break;
        }
    }
    return validMotion;
}

bool GCodeParser::AddLine(const char* ptr)
{
    bool res = ParseLine(ptr);
    if (res) {
        if (lastState.cmd == eDril) {
            // split to several motions
            lastState.cmd = eMoveLiner;
            float rPlane;
            if (lastState.retract_mode == 99) {
                rPlane = lastState.r;
            }
            else {
                rPlane = lastState.retract_z;
            }
            float finalDepth = lastState.z;
            lastState.z = rPlane;
            Operations.push_back(lastState);
            lastState.z = finalDepth;
            Operations.push_back(lastState);
            lastState.z = rPlane;
            Operations.push_back(lastState);
            lastState.cmd = eDril;
        }
        else {
            Operations.push_back(lastState);
        }
    }
    return res;
}
