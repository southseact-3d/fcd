#ifndef UNITSCHEMASPECS_H
#define UNITSCHEMASPECS_H

#include <map>
#include <string>
#include <vector>
#include <functional>

namespace Base
{


struct UnitTranslationSpec
{
    double threshold {1};
    std::string unitString;
    double factor {1};
    std::function<std::string(double)> fn {nullptr};
};

struct UnitsSchemaSpec
{
    std::size_t num;
    std::string name;
    std::string basicLengthUnitStr;
    bool isMultUnitLen {false};
    bool isMultUnitAngle {false};
    const char* description;
    bool isDefault {false};

    /**
     * Applicable spec is the first with threshold > value under test
     * Special case: Threshold = 0 : default
     * Special case: Factor = 0 : unitString contains name of special function to run
     */
    std::map<std::string, std::vector<UnitTranslationSpec>> translationSpecs;
};

struct UnitsSchemasDataPack
{
    std::vector<UnitsSchemaSpec> specs;
    size_t defDecimals;
    size_t defDenominator;
};


}  // namespace Base
#endif  // UNITSCHEMASPECS_H
