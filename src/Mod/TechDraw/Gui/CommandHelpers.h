#ifndef COMMANDHELPERS_H
#define COMMANDHELPERS_H

#include <string>
#include <vector>

#include <Mod/TechDraw/TechDrawGlobal.h>
#include <Base/Vector3D.h>

namespace App {
class DocumentObject;
}

namespace Gui {
class Command;
}

namespace TechDraw {
class DrawView;
class DrawViewPart;

/**
 * CommandHelpers is a collection of methods for common actions in commands.
 */
namespace CommandHelpers {
TechDraw::DrawView* firstViewInSelection(Gui::Command* cmd);
TechDraw::DrawView* firstNonSpreadsheetInSelection(Gui::Command* cmd);

std::vector<std::string> getSelectedSubElements(Gui::Command* cmd,
                                                TechDraw::DrawViewPart* &dvp,
                                                std::string subType = "Edge");


std::pair<App::DocumentObject*, std::string> faceFromSelection();


}   // end namespace CommandHelpers
}   // end namespace TechDraw

#endif
