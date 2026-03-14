#ifndef SKETCHERGUI_PropertyVisualLayerList_H
#define SKETCHERGUI_PropertyVisualLayerList_H

#include <vector>

#include <App/Property.h>

#include <Mod/Sketcher/SketcherGlobal.h>

#include "VisualLayer.h"


namespace Base
{
class Writer;
}

namespace SketcherGui
{

class SketcherGuiExport PropertyVisualLayerList: public App::PropertyListsT<VisualLayer>
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /**
     * A constructor.
     * A more elaborate description of the constructor.
     */
    PropertyVisualLayerList();

    /**
     * A destructor.
     * A more elaborate description of the destructor.
     */
    ~PropertyVisualLayerList() override;

    PyObject* getPyObject() override;

    void Save(Base::Writer& writer) const override;
    void Restore(Base::XMLReader& reader) override;

    Property* Copy() const override;
    void Paste(const Property& from) override;
    unsigned int getMemSize() const override;

protected:
    VisualLayer getPyValue(PyObject*) const override;
};

}  // namespace SketcherGui


#endif  // SKETCHERGUI_PropertyVisualLayerList_H
