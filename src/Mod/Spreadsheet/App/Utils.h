#ifndef UTILS_H
#define UTILS_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include <Mod/Spreadsheet/SpreadsheetGlobal.h>


namespace Spreadsheet
{

SpreadsheetExport std::string columnName(int col);
SpreadsheetExport std::string rowName(int row);

SpreadsheetExport void createRectangles(
    std::set<std::pair<int, int>>& cells,
    std::map<std::pair<int, int>, std::pair<int, int>>& rectangles
);
SpreadsheetExport std::string quote(const std::string& input);
SpreadsheetExport std::string unquote(const std::string& input);

}  // namespace Spreadsheet

#endif  // UTILS_H
