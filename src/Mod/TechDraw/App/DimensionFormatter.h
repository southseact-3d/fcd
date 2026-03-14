#ifndef DIMENSIONFORMATTER_H
#define DIMENSIONFORMATTER_H

#include <QString>

#include <Mod/TechDraw/TechDrawGlobal.h>

//#include <DrawViewDimension.h> Cyclic dependency issue!

class QString;

namespace TechDraw {
class DrawViewDimension;

//TODO: Why is this a class if it has no state???
class TechDrawExport DimensionFormatter {
public:
    enum class Format : int {
        UNALTERED, // return the unaltered user string from the Units subsystem
        FORMATTED, // return value formatted according to the format spec and
                   // preferences for useAltDecimals and showUnits
        UNIT       // return only the unit of measure
    };

    DimensionFormatter() = default;
    explicit DimensionFormatter(DrawViewDimension* dim)
        : m_dimension {dim}
    {}
    ~DimensionFormatter() = default;

    //void setDimension(DrawViewDimension* dim) { m_dimension = dim; }
    bool isMultiValueSchema() const;
    std::string formatValue(qreal value, const QString& qFormatSpec, Format partial, bool isDim) const;
    std::string getFormattedToleranceValue(Format partial) const;
    std::pair<std::string, std::string> getFormattedToleranceValues(Format partial) const;
    std::string getFormattedDimensionValue(Format partial) const;
    QStringList getPrefixSuffixSpec(const QString& fSpec) const;
    std::string getDefaultFormatSpec(bool isToleranceFormat) const;

private:
    QString formatValueToSpec(double value, QString formatSpecifier) const;
    bool isNumericFormat(const QString& formatSpecifier) const;

    DrawViewDimension* m_dimension {nullptr};
};

}  // end namespace TechDraw
#endif
