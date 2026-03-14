#ifndef MEASURE_SUBNAMEMANIPULATOR_H
#define MEASURE_SUBNAMEMANIPULATOR_H

#include <Mod/Measure/MeasureGlobal.h>

#include <TopoDS_Shape.hxx>

#include <App/DocumentObject.h>
#include <App/DocumentObserver.h>
#include <Base/Placement.h>
#include <Base/Matrix.h>

#include <Mod/Part/App/TopoShape.h>

namespace Measure
{

//! a class to perform common operations on subelement names.
class MeasureExport SubnameHelper
{
public:
    static std::string getLastTerm(const std::string& inString);
    static std::string getFirstTerm(const std::string& inString);
    static std::string namesToLongSub(const std::vector<std::string>& pathElementNames);
    static std::string pruneLastTerm(const std::string& inString);
    static std::string pruneFirstTerm(const std::string& inString);
    static std::string removeGeometryTerm(const std::string& longSubname);
    static std::string pathToLongSub(std::list<App::DocumentObject*> path);
    static std::string removeTnpInfo(const std::string& inString);
};

}  // namespace Measure

#endif  // MEASURE_SUBNAMEMANIPULATOR_H
