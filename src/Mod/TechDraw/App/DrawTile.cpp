#include "DrawTile.h"
#include "DrawTilePy.h"  // generated from DrawTilePy.xml


using namespace TechDraw;

//===========================================================================
// DrawTile - attachable tile
//===========================================================================

PROPERTY_SOURCE(TechDraw::DrawTile, App::DocumentObject)

DrawTile::DrawTile()
{
    static const char *group = "Tile";

    ADD_PROPERTY_TYPE(TileParent, (nullptr), group, (App::PropertyType)(App::Prop_None),
                      "Object to which this tile is attached");
    ADD_PROPERTY_TYPE(TileRow, (0), group, App::Prop_None, "Row in parent object\n 0 for arrow side, -1 for other side");
    ADD_PROPERTY_TYPE(TileColumn, (0), group, App::Prop_None, "Column in parent object");

    // there is currently only one column, this don't allow one to edit
    TileColumn.setStatus(App::Property::ReadOnly, true);
    // the row can only have the value 0 or -1
    // allow its editing because this way the tiles can be flipped
    TileRowConstraints.LowerBound = -1;
    TileRowConstraints.UpperBound = 0;
    TileRowConstraints.StepSize = 1;
    TileRow.setConstraints(&TileRowConstraints);
}

void DrawTile::onChanged(const App::Property* prop)
{
    if (!isRestoring()) {
        //nothing in particular
    }
    DocumentObject::onChanged(prop);

}

short DrawTile::mustExecute() const
{
    return DocumentObject::mustExecute();
}

App::DocumentObjectExecReturn *DrawTile::execute()
{
//    Base::Console().message("DT::execute()\n");
    return DocumentObject::execute();
}

void DrawTile::handleChangedPropertyType(Base::XMLReader &reader, const char *TypeName, App::Property *prop)
// transforms properties that had been changed
{
    // property TileRow had App::PropertyInteger and was changed to App::PropertyIntegerConstraint
    if (prop == &TileRow && strcmp(TypeName, "App::PropertyInteger") == 0) {
        App::PropertyInteger TileRowProperty;
        // restore the PropertyInteger to be able to set its value
        TileRowProperty.Restore(reader);
        TileRow.setValue(TileRowProperty.getValue());
    }
}

DrawView* DrawTile::getParent() const
{
//    Base::Console().message("DT::getParent() - %s\n", getNameInDocument());
    return freecad_cast<DrawView*>(TileParent.getValue());
}

PyObject *DrawTile::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new DrawTilePy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

// Python Drawing feature ---------------------------------------------------------

namespace App {
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(TechDraw::DrawTilePython, TechDraw::DrawTile)
template<> const char* TechDraw::DrawTilePython::getViewProviderName() const {
    return "TechDrawGui::ViewProviderTile";
}
/// @endcond

// explicit template instantiation
template class TechDrawExport FeaturePythonT<TechDraw::DrawTile>;
}

