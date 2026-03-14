#ifndef SKETCHER_PythonConverter_H
#define SKETCHER_PythonConverter_H

#include <string>
#include <vector>

#include <Mod/Sketcher/SketcherGlobal.h>

namespace Part
{
class Geometry;
}

namespace Sketcher
{
class Constraint;

/** @brief      Class for generating python code
 *  @details
 *  Given C++ structures, it generates the python code that should be written in the console to
 *  create such objects.
 */

class SketcherExport PythonConverter
{
    class SingleGeometry
    {
    public:
        std::string creation;
        bool construction;
    };

public:
    enum class Mode
    {
        CreateInternalGeometry,
        OmitInternalGeometry
    };

    enum class GeoIdMode
    {
        DoNotChangeGeoIds,
        AddLastGeoIdToGeoIds,
    };

    PythonConverter() = delete;
    ~PythonConverter() = delete;

    /// Convert a geometry into the string representing the command creating it
    static std::string convert(const Part::Geometry* geo, Mode mode = Mode::CreateInternalGeometry);

    /// Convert a vector of geometries into the string representing the command creating them
    static std::string convert(
        const std::string& doc,
        const std::vector<Part::Geometry*>& geos,
        Mode mode = Mode::CreateInternalGeometry
    );

    static std::string convert(
        const Sketcher::Constraint* constraint,
        GeoIdMode geoIdMode = GeoIdMode::DoNotChangeGeoIds
    );

    static std::string convert(
        const std::string& doc,
        const std::vector<Sketcher::Constraint*>& constraints,
        GeoIdMode geoIdMode = GeoIdMode::DoNotChangeGeoIds
    );

    static std::vector<std::string> multiLine(std::string&& singlestring);

private:
    static SingleGeometry process(const Part::Geometry* geo);

    static std::string process(
        const Sketcher::Constraint* constraint,
        GeoIdMode geoIdMode = GeoIdMode::DoNotChangeGeoIds
    );
};

}  // namespace Sketcher


#endif  // SKETCHER_PythonConverter_H
