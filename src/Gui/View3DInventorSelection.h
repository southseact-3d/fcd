#ifndef GUI_VIEW3DINVENTORSELECTION_H
#define GUI_VIEW3DINVENTORSELECTION_H

#include <map>
#include <string>
#include <Gui/Selection/Selection.h>

class SoGroup;
class SoNode;
class SoSeparator;

namespace Gui
{

class Document;
class SoFCUnifiedSelection;

class GuiExport View3DInventorSelection
{
public:
    View3DInventorSelection(SoFCUnifiedSelection* root);
    ~View3DInventorSelection();

    void setDocument(Gui::Document* pcDocument)
    {
        guiDocument = pcDocument;
    }
    Gui::Document* getDocument() const
    {
        return guiDocument;
    }

    void checkGroupOnTop(const SelectionChanges& Reason);
    void clearGroupOnTop();

private:
    SoGroup* pcGroupOnTop;
    SoGroup* pcGroupOnTopSel;
    SoGroup* pcGroupOnTopPreSel;
    SoFCUnifiedSelection* selectionRoot;
    std::map<std::string, SoNode*> objectsOnTop;
    std::map<std::string, SoNode*> objectsOnTopPreSel;
    Gui::Document* guiDocument = nullptr;
};

}  // namespace Gui

#endif  // GUI_VIEW3DINVENTORSELECTION_H
