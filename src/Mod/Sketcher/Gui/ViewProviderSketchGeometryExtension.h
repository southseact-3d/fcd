#ifndef SKETCHER_VIEWPROVIDERSKETCHGEOMETRYEXTENSION_H
#define SKETCHER_VIEWPROVIDERSKETCHGEOMETRYEXTENSION_H

#include <Mod/Part/App/Geometry.h>
#include <Mod/Sketcher/SketcherGlobal.h>


namespace SketcherGui
{

class SketcherGuiExport ViewProviderSketchGeometryExtension: public Part::GeometryPersistenceExtension
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    ViewProviderSketchGeometryExtension();
    ~ViewProviderSketchGeometryExtension() override = default;

    std::unique_ptr<Part::GeometryExtension> copy() const override;

    PyObject* getPyObject() override;

    // Data Members

    // Representation factor
    // Provides a mechanism to store a factor associated with the representation of a geometry
    // This is only useful when a geometry must be scaled only for representation, while keeping its
    // value Applicability: General abstract concepts embodied in a geometry, in practice B-Spline
    // poles. Why not in SketchGeometryExtension? Because it is merely representation related. It
    // has no place in a console application.
    double getRepresentationFactor() const
    {
        return RepresentationFactor;
    }
    void setRepresentationFactor(double representationFactor)
    {
        RepresentationFactor = representationFactor;
    }

    int getVisualLayerId() const
    {
        return VisualLayerId;
    }
    void setVisualLayerId(int visuallayerid)
    {
        VisualLayerId = visuallayerid;
    }

protected:
    void copyAttributes(Part::GeometryExtension* cpy) const override;
    void restoreAttributes(Base::XMLReader& reader) override;
    void saveAttributes(Base::Writer& writer) const override;

private:
    ViewProviderSketchGeometryExtension(const ViewProviderSketchGeometryExtension&) = default;

private:
    double RepresentationFactor;
    int VisualLayerId;
};

}  // namespace SketcherGui


#endif  // SKETCHER_VIEWPROVIDERSKETCHGEOMETRYEXTENSION_H
