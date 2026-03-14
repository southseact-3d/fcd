#ifndef PART_PRISM_EXTENSION_H
#define PART_PRISM_EXTENSION_H

#include <App/DocumentObjectExtension.h>
#include <App/PropertyUnits.h>
#include <Mod/Part/PartGlobal.h>

#include <TopoDS_Face.hxx>


namespace Part
{

class PartExport PrismExtension: public App::DocumentObjectExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Part::PrismExtension);

public:
    PrismExtension();
    ~PrismExtension() override;


    App::PropertyAngle FirstAngle;
    App::PropertyAngle SecondAngle;

    TopoDS_Shape makePrism(double height, const TopoDS_Face& face) const;

    short int extensionMustExecute() override;
    App::DocumentObjectExecReturn* extensionExecute() override;

protected:
    void extensionOnChanged(const App::Property* /*prop*/) override;
};

}  // namespace Part

#endif  // PART_PRISM_EXTENSION_H
