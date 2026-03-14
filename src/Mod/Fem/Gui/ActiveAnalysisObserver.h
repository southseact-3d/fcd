#ifndef FEMGUI_ACTIVEANALYSISOBSERVER_H
#define FEMGUI_ACTIVEANALYSISOBSERVER_H

#include <App/DocumentObserver.h>
#include <Gui/Tree.h>

namespace Gui
{
class Document;
class ViewProviderDocumentObject;
}  // namespace Gui

namespace Fem
{
class FemAnalysis;
}

namespace FemGui
{

class ActiveAnalysisObserver: public App::DocumentObserver
{
public:
    static ActiveAnalysisObserver* instance();

    void setActiveObject(Fem::FemAnalysis*);
    Fem::FemAnalysis* getActiveObject() const;
    bool hasActiveObject() const;
    void highlightActiveObject(const Gui::HighlightMode&, bool);

private:
    ActiveAnalysisObserver();
    ~ActiveAnalysisObserver() override;

    void slotDeletedDocument(const App::Document& Doc) override;
    void slotDeletedObject(const App::DocumentObject& Obj) override;

private:
    static ActiveAnalysisObserver* inst;
    Fem::FemAnalysis* activeObject {nullptr};
    Gui::ViewProviderDocumentObject* activeView {nullptr};
    Gui::Document* activeDocument {nullptr};
};

}  // namespace FemGui

#endif  // FEMGUI_ACTIVEANALYSISOBSERVER_H
