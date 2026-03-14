#include <vector>


#include <App/Document.h>
#include <App/DocumentObject.h>

#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>

#include <Base/Interpreter.h>

#include "ViewProviderBom.h"

using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderBom, SpreadsheetGui::ViewProviderSheet)

ViewProviderBom::ViewProviderBom()
{}

ViewProviderBom::~ViewProviderBom() = default;

QIcon ViewProviderBom::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Assembly_BillOfMaterials.svg");
}

bool ViewProviderBom::doubleClicked()
{
    std::string obj_name = getObject()->getNameInDocument();
    std::string doc_name = getObject()->getDocument()->getName();

    std::string pythonCommand = "import CommandCreateBom\n"
                                "obj = App.getDocument('"
        + doc_name + "').getObject('" + obj_name
        + "')\n"
          "Gui.Control.showDialog(CommandCreateBom.TaskAssemblyCreateBom(obj))";

    Gui::Command::runCommand(Gui::Command::App, pythonCommand.c_str());

    return true;
}
