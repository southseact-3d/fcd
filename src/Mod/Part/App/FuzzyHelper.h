#ifndef PART_FUZZYHELPER_H
#define PART_FUZZYHELPER_H

#include <Mod/Part/PartGlobal.h>
#include <functional>

namespace Part
{

/**
 * @brief The FuzzyHelper provides helper functions to determine fuzzy value for a boolean operation
 */
namespace FuzzyHelper
{
double PartExport getBooleanFuzzy();
void PartExport setBooleanFuzzy(double base);
void PartExport withBooleanFuzzy(double base, std::function<void()> func);
}  // namespace FuzzyHelper

}  // namespace Part

#endif  // PART_FUZZYHELPER_H
