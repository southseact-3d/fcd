#ifndef BASE_UNITSSCHEMA_H
#define BASE_UNITSSCHEMA_H

#include <string>
#include <memory>

#include "UnitsSchemasSpecs.h"
#include "Base/Quantity.h"

namespace Base
{
class Quantity;

/**
 * An individual schema object
 */
class BaseExport UnitsSchema
{
public:
    explicit UnitsSchema(UnitsSchemaSpec spec);
    UnitsSchema() = delete;

    [[nodiscard]] bool isMultiUnitLength() const;
    [[nodiscard]] bool isMultiUnitAngle() const;
    [[nodiscard]] std::string getBasicLengthUnit() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::string getDescription() const;
    [[nodiscard]] int getNum() const;

    std::string translate(const Quantity& quant) const;
    std::string translate(const Quantity& quant, double& factor, std::string& unitString) const;

private:
    [[nodiscard]] static std::string toLocale(
        const Quantity& quant,
        double factor,
        const std::string& unitString
    );

    UnitsSchemaSpec spec;
};


}  // namespace Base
#endif  // BASE_UNITSSCHEMA_H
