#include <App/Application.h>
#include <App/Document.h>
#include <App/FeaturePythonPyImp.h>
#include <App/PropertyPythonObject.h>
#include <Base/Console.h>
#include <Base/Tools.h>

#include "JointGroup.h"
#include "JointGroupPy.h"

using namespace Assembly;


PROPERTY_SOURCE(Assembly::JointGroup, App::DocumentObjectGroup)

JointGroup::JointGroup()
{}

JointGroup::~JointGroup() = default;

PyObject* JointGroup::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new JointGroupPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}


std::vector<App::DocumentObject*> JointGroup::getJoints()
{
    std::vector<App::DocumentObject*> joints = {};

    Base::PyGILStateLocker lock;
    for (auto joint : getObjects()) {
        if (!joint) {
            continue;
        }

        auto* prop = dynamic_cast<App::PropertyBool*>(joint->getPropertyByName("Suppressed"));
        if (!prop || prop->getValue()) {
            // Filter grounded joints and deactivated joints.
            continue;
        }

        auto proxy = dynamic_cast<App::PropertyPythonObject*>(joint->getPropertyByName("Proxy"));
        if (proxy) {
            if (proxy->getValue().hasAttr("setJointConnectors")) {
                joints.push_back(joint);
            }
        }
    }

    return joints;
}
