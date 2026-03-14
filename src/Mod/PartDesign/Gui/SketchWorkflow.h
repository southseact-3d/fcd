#ifndef PARTDESIGNGUI_SKETCHWORKFLOW_H
#define PARTDESIGNGUI_SKETCHWORKFLOW_H

#include <tuple>
#include <Mod/PartDesign/PartDesignGlobal.h>
#include <Gui/Selection/SelectionFilter.h>

namespace App
{
class Document;
class DocumentObject;
class GeoFeatureGroupExtension;
}  // namespace App
namespace Gui
{
class Document;
}
namespace PartDesign
{
class Body;
}
namespace PartDesignGui
{

class SketchWorkflow
{
public:
    explicit SketchWorkflow(Gui::Document*);
    void createSketch();

private:
    void tryCreateSketch();
    std::tuple<bool, PartDesign::Body*> shouldCreateBody();
    bool shouldAbort(bool) const;
    std::tuple<Gui::SelectionFilter, Gui::SelectionFilter, Gui::SelectionFilter> getFilters() const;

private:
    Gui::Document* guidocument;
    App::Document* appdocument;
    PartDesign::Body* activeBody {nullptr};
};

}  // namespace PartDesignGui

#endif  // PARTDESIGNGUI_SKETCHWORKFLOW_H
