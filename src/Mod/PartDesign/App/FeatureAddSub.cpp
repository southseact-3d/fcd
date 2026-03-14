#include <Standard_Failure.hxx>


#include <App/FeaturePythonPyImp.h>
#include <Mod/Part/App/modelRefine.h>
#include <Mod/Part/App/TopoShapeOpCode.h>

#include "FeatureAddSub.h"
#include "FeaturePy.h"

#include <Mod/Part/App/Tools.h>


using namespace PartDesign;

namespace PartDesign
{

extern bool getPDRefineModelParameter();

PROPERTY_SOURCE(PartDesign::FeatureAddSub, PartDesign::FeatureRefine)

FeatureAddSub::FeatureAddSub()
{
    ADD_PROPERTY(AddSubShape, (TopoDS_Shape()));
}

void FeatureAddSub::onChanged(const App::Property* property)
{
    Feature::onChanged(property);
}

FeatureAddSub::Type FeatureAddSub::getAddSubType()
{
    return addSubType;
}

short FeatureAddSub::mustExecute() const
{
    if (Refine.isTouched()) {
        return 1;
    }
    return PartDesign::Feature::mustExecute();
}

void FeatureAddSub::getAddSubShape(Part::TopoShape& addShape, Part::TopoShape& subShape)
{
    if (addSubType == Additive) {
        addShape = AddSubShape.getShape();
    }
    else if (addSubType == Subtractive) {
        subShape = AddSubShape.getShape();
    }
}

void FeatureAddSub::updatePreviewShape()
{
    const auto notifyWarning = [](const QString& message) {
        Base::Console().translatedUserWarning(
            "Preview",
            tr("Failure while computing removed volume preview: %1").arg(message).toUtf8()
        );
    };

    // for subtractive shapes we want to also showcase removed volume, not only the tool
    if (addSubType == Subtractive) {
        TopoShape base = getBaseTopoShape(true).moved(getLocation().Inverted());

        if (const TopoShape& addSubShape = AddSubShape.getShape(); !addSubShape.isEmpty()) {
            try {
                base.makeElementBoolean(Part::OpCodes::Common, {base, addSubShape});
            }
            catch (Standard_Failure& e) {
                notifyWarning(QString::fromUtf8(e.GetMessageString()));
            }
            catch (Base::Exception& e) {
                notifyWarning(QString::fromStdString(e.getMessage()));
            }

            if (base.isEmpty()) {
                notifyWarning(
                    tr("Resulting shape is empty. That may indicate that no material will be "
                       "removed or a problem with the model.")
                );
            }

            PreviewShape.setValue(base);
            return;
        }
    }

    PreviewShape.setValue(AddSubShape.getShape());
}

}  // namespace PartDesign

namespace App
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(PartDesign::FeatureAddSubPython, PartDesign::FeatureAddSub)
template<>
const char* PartDesign::FeatureAddSubPython::getViewProviderName() const
{
    return "PartDesignGui::ViewProviderPython";
}
template<>
PyObject* PartDesign::FeatureAddSubPython::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new FeaturePythonPyT<PartDesign::FeaturePy>(this), true);
    }
    return Py::new_reference_to(PythonObject);
}
/// @endcond

// explicit template instantiation
template class PartDesignExport FeaturePythonT<PartDesign::FeatureAddSub>;
}  // namespace App


namespace PartDesign
{

PROPERTY_SOURCE(PartDesign::FeatureAdditivePython, PartDesign::FeatureAddSubPython)

FeatureAdditivePython::FeatureAdditivePython()
{
    addSubType = Additive;
}

FeatureAdditivePython::~FeatureAdditivePython() = default;


PROPERTY_SOURCE(PartDesign::FeatureSubtractivePython, PartDesign::FeatureAddSubPython)

FeatureSubtractivePython::FeatureSubtractivePython()
{
    addSubType = Subtractive;
}

FeatureSubtractivePython::~FeatureSubtractivePython() = default;

}  // namespace PartDesign
