#include <iomanip>
#include <sstream>
#include <string>

#include <QLocale>
#include <QString>

#include "Quantity.h"
#include "UnitsSchema.h"
#include "UnitsSchemasData.h"
#include "UnitsSchemasSpecs.h"
#include "Exception.h"
#include "Quantity.h"

using Base::UnitsSchema;
using Base::UnitsSchemaSpec;


UnitsSchema::UnitsSchema(UnitsSchemaSpec spec)
    : spec {std::move(spec)}
{}

std::string UnitsSchema::translate(const Quantity& quant) const
{  // to satisfy GCC
    double dummy1 {};
    std::string dummy2;
    return translate(quant, dummy1, dummy2);
}

std::string UnitsSchema::translate(const Quantity& quant, double& factor, std::string& unitString) const
{
    // Use defaults without schema-level translation.
    factor = 1.0;
    unitString = quant.getUnit().getString();

    if (spec.translationSpecs.empty()) {
        return toLocale(quant, factor, unitString);
    }

    const auto unitName = quant.getUnit().getTypeString();
    if (!spec.translationSpecs.contains(unitName)) {
        return toLocale(quant, factor, unitString);
    }

    const auto value = quant.getValue();
    auto isSuitable = [&](const UnitTranslationSpec& row) {
        // Shrink threshold slightly so values at exact threshold boundaries
        // (e.g. "1 S/m" = 1e-9 at threshold 1e-9) fall through to the next unit.
        constexpr double relEps = 1e-12;
        return row.threshold * (1.0 - relEps) > value
            || row.threshold == 0;  // zero indicates default
    };

    auto unitSpecs = spec.translationSpecs.at(unitName);
    const auto unitSpec = std::find_if(unitSpecs.begin(), unitSpecs.end(), isSuitable);
    if (unitSpec == unitSpecs.end()) {
        throw RuntimeError(
            "Suitable threshold not found. Schema: " + spec.name + " value: " + std::to_string(value)
        );
    }

    if (unitSpec->factor == 0) {
        const QuantityFormat& format = quant.getFormat();
        return UnitsSchemasData::runSpecial(
            unitSpec->unitString,
            value,
            format.getPrecision(),
            format.getDenominator(),
            factor,
            unitString
        );
    }

    factor = unitSpec->factor;
    unitString = unitSpec->unitString;

    return toLocale(quant, factor, unitString);
}

std::string UnitsSchema::toLocale(const Quantity& quant, const double factor, const std::string& unitString)
{
    QLocale Lc;
    const QuantityFormat& format = quant.getFormat();
    if (format.option != QuantityFormat::None) {
        Lc.setNumberOptions(static_cast<QLocale::NumberOptions>(format.option));
    }

    auto valueString = Lc.toString(quant.getValue() / factor, format.toFormat(), format.getPrecision())
                           .toStdString();

    auto notUnit = [](auto s) {
        return s.empty() || s == "°" || s == "″" || s == "′" || s == "\"" || s == "'";
    };

    return fmt::format("{}{}{}", valueString, notUnit(unitString) ? "" : " ", unitString);
}

bool UnitsSchema::isMultiUnitLength() const
{
    return spec.isMultUnitLen;
}

bool UnitsSchema::isMultiUnitAngle() const
{
    return spec.isMultUnitAngle;
}

std::string UnitsSchema::getBasicLengthUnit() const
{
    return spec.basicLengthUnitStr;
}

std::string UnitsSchema::getName() const
{
    return spec.name;
}

std::string UnitsSchema::getDescription() const
{
    return spec.description;
}

int UnitsSchema::getNum() const
{
    return static_cast<int>(spec.num);
}
