#ifndef SURFACE_FEATURESEWING_H
#define SURFACE_FEATURESEWING_H

#include <App/PropertyStandard.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Surface/SurfaceGlobal.h>


namespace Surface
{

class SurfaceExport Sewing: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Surface::Sewing);

public:
    Sewing();

    App::PropertyLinkSubList ShapeList;  // Shapes to be sewn.

    App::PropertyFloat Tolerance;
    App::PropertyBool SewingOption;     // Option for sewing (if false only control)
    App::PropertyBool DegenerateShape;  // Option for analysis of degenerated shapes
    App::PropertyBool CutFreeEdges;     // Option for cutting of free edges
    App::PropertyBool Nonmanifold;      // Option for non-manifold processing

    // recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
};

}  // Namespace Surface

#endif  // SURFACE_FEATURESEWING_H
