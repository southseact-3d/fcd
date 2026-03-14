#ifndef SKETCH_SKETCHOBJECTSF_H
#define SKETCH_SKETCHOBJECTSF_H

#include <App/PropertyFile.h>
#include <Mod/Part/App/Part2DObject.h>
#include <Mod/Sketcher/SketcherGlobal.h>


namespace Sketcher
{

class SketchObjectSF: public Part::Part2DObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Sketcher::SketchObjectSF);

public:
    SketchObjectSF();

    /// Property
    App::PropertyFileIncluded SketchFlatFile;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// Uses the standard ViewProvider
    // const char* getViewProviderName(void) const {
    //     return "SketcherGui::ViewProviderSketchSF";
    // }
    //@}

    bool save(const char* FileName);
    bool load(const char* FileName);
};

}  // namespace Sketcher


#endif  // SKETCH_SKETCHOBJECTSF_H
