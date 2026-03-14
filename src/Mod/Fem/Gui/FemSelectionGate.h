#ifndef GUI_FemSelectionGate_H
#define GUI_FemSelectionGate_H

#include <Gui/Selection/SelectionFilter.h>

namespace FemGui
{

class FemSelectionGate: public Gui::SelectionFilterGate
{
public:
    enum ElemType
    {
        Nothing,
        Node,
        Element,
        NodeElement
    };

    explicit FemSelectionGate(ElemType type)
        : Gui::SelectionFilterGate(nullPointer())
        , Type(type)
    {}

    ElemType Type;

    /// get called by the frame-work
    bool allow(App::Document* pDoc, App::DocumentObject* pObj, const char* sSubName) override;
};

}  // namespace FemGui

#endif  // GUI_FemSelectionGate_H
