#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include "PropertyVisualLayerList.h"


using namespace App;
using namespace Base;
using namespace std;
using namespace SketcherGui;


TYPESYSTEM_SOURCE(SketcherGui::PropertyVisualLayerList, App::PropertyLists)

//**************************************************************************
// Construction/Destruction

PropertyVisualLayerList::PropertyVisualLayerList() = default;

PropertyVisualLayerList::~PropertyVisualLayerList() = default;

//**************************************************************************
// Base class implementer

PyObject* PropertyVisualLayerList::getPyObject()
{
    THROWM(Base::NotImplementedError, "PropertyVisualLayerList has no python counterpart");
}

VisualLayer PropertyVisualLayerList::getPyValue(PyObject* item) const
{

    (void)item;
    THROWM(Base::NotImplementedError, "PropertyVisualLayerList has no python counterpart");
}

void PropertyVisualLayerList::Save(Base::Writer& writer) const
{
    writer.Stream() << writer.ind() << "<VisualLayerList count=\"" << getSize() << "\">" << endl;
    writer.incInd();
    for (int i = 0; i < getSize(); i++) {
        _lValueList[i].Save(writer);
    }
    writer.decInd();
    writer.Stream() << writer.ind() << "</VisualLayerList>" << endl;
}

void PropertyVisualLayerList::Restore(Base::XMLReader& reader)
{
    reader.readElement("VisualLayerList");
    // get the value of my attribute
    int count = reader.getAttribute<long>("count");

    std::vector<VisualLayer> layers;
    layers.reserve(count);

    for (int i = 0; i < count; i++) {
        VisualLayer visualLayer;
        visualLayer.Restore(reader);
        layers.push_back(std::move(visualLayer));
    }

    reader.readEndElement("VisualLayerList");

    setValues(std::move(layers));
}

Property* PropertyVisualLayerList::Copy() const
{
    PropertyVisualLayerList* p = new PropertyVisualLayerList();
    p->_lValueList = _lValueList;
    return p;
}

void PropertyVisualLayerList::Paste(const Property& from)
{
    setValues(dynamic_cast<const PropertyVisualLayerList&>(from)._lValueList);
}

unsigned int PropertyVisualLayerList::getMemSize() const
{
    return static_cast<unsigned int>(_lValueList.size() * sizeof(VisualLayer));
}
