#include <sstream>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Selection/SelectionObjectPy.h>

#include "SelectionObject.h"
#include "Selection.h"
#include "Application.h"
#include "Command.h"


using namespace Gui;


TYPESYSTEM_SOURCE_ABSTRACT(Gui::SelectionObject, Base::BaseClass)

SelectionObject::SelectionObject() = default;

SelectionObject::SelectionObject(const Gui::SelectionChanges& msg)
{
    FeatName = msg.pObjectName ? msg.pObjectName : "";
    DocName = msg.pDocName ? msg.pDocName : "";
    TypeName = msg.pTypeName ? msg.pTypeName : "";
    if (msg.pSubName) {
        SubNames.emplace_back(msg.pSubName);
        SelPoses.emplace_back(msg.x, msg.y, msg.z);
    }
}

SelectionObject::SelectionObject(const App::DocumentObject* obj)
{
    FeatName = obj->getNameInDocument();
    DocName = obj->getDocument()->getName();
    TypeName = obj->getTypeId().getName();
}

SelectionObject::~SelectionObject() = default;

const App::DocumentObject* SelectionObject::getObject() const
{
    if (!DocName.empty()) {
        App::Document* doc = App::GetApplication().getDocument(DocName.c_str());
        if (doc && !FeatName.empty()) {
            return doc->getObject(FeatName.c_str());
        }
    }
    return nullptr;
}

App::DocumentObject* SelectionObject::getObject()
{
    if (!DocName.empty()) {
        App::Document* doc = App::GetApplication().getDocument(DocName.c_str());
        if (doc && !FeatName.empty()) {
            return doc->getObject(FeatName.c_str());
        }
    }
    return nullptr;
}

bool SelectionObject::isObjectTypeOf(const Base::Type& typeId) const
{
    const App::DocumentObject* obj = getObject();
    return (obj && obj->isDerivedFrom(typeId));
}

std::string SelectionObject::getAsPropertyLinkSubString() const
{
    std::ostringstream str;
    str << "(" << Gui::Command::getObjectCmd(getObject()) << ",[";
    for (const auto& it : SubNames) {
        str << "'" << it << "',";
    }
    str << "])";
    return str.str();
}

PyObject* SelectionObject::getPyObject()
{
    return new SelectionObjectPy(new SelectionObject(*this));
}
