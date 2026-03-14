#include <Base/Exception.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include <Mod/Sketcher/Gui/ViewProviderSketchGeometryExtensionPy.h>

#include "ViewProviderSketchGeometryExtension.h"


using namespace SketcherGui;

//---------- Geometry Extension
TYPESYSTEM_SOURCE(SketcherGui::ViewProviderSketchGeometryExtension, Part::GeometryPersistenceExtension)


ViewProviderSketchGeometryExtension::ViewProviderSketchGeometryExtension()
    : RepresentationFactor(1.0)
    , VisualLayerId(0)
{}

void ViewProviderSketchGeometryExtension::copyAttributes(Part::GeometryExtension* cpy) const
{
    Part::GeometryExtension::copyAttributes(cpy);
    static_cast<ViewProviderSketchGeometryExtension*>(cpy)->RepresentationFactor
        = this->RepresentationFactor;
    static_cast<ViewProviderSketchGeometryExtension*>(cpy)->VisualLayerId = this->VisualLayerId;
}

std::unique_ptr<Part::GeometryExtension> ViewProviderSketchGeometryExtension::copy() const
{
    auto cpy = std::make_unique<ViewProviderSketchGeometryExtension>();

    copyAttributes(cpy.get());

    return cpy;
}

void ViewProviderSketchGeometryExtension::restoreAttributes(Base::XMLReader& reader)
{
    Part::GeometryPersistenceExtension::restoreAttributes(reader);

    if (reader.hasAttribute("visualLayerId")) {
        VisualLayerId = reader.getAttribute<long>("visualLayerId");
    }
}

void ViewProviderSketchGeometryExtension::saveAttributes(Base::Writer& writer) const
{
    Part::GeometryPersistenceExtension::saveAttributes(writer);

    writer.Stream() << "\" visualLayerId=\"" << VisualLayerId;
}


PyObject* ViewProviderSketchGeometryExtension::getPyObject()
{
    return new ViewProviderSketchGeometryExtensionPy(new ViewProviderSketchGeometryExtension(*this));
}
