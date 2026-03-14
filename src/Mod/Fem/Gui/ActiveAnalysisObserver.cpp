#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/Fem/App/FemAnalysis.h>

#include "ActiveAnalysisObserver.h"


using namespace FemGui;

ActiveAnalysisObserver* ActiveAnalysisObserver::inst = nullptr;

ActiveAnalysisObserver* ActiveAnalysisObserver::instance()
{
    if (!inst) {
        inst = new ActiveAnalysisObserver();
    }
    return inst;
}

ActiveAnalysisObserver::ActiveAnalysisObserver() = default;

ActiveAnalysisObserver::~ActiveAnalysisObserver() = default;

void ActiveAnalysisObserver::setActiveObject(Fem::FemAnalysis* fem)
{
    if (fem) {
        activeObject = fem;
        App::Document* doc = fem->getDocument();
        activeDocument = Gui::Application::Instance->getDocument(doc);
        activeView = static_cast<Gui::ViewProviderDocumentObject*>(
            activeDocument->getViewProvider(activeObject)
        );
        attachDocument(doc);
    }
    else {
        activeObject = nullptr;
        activeView = nullptr;
    }
}

Fem::FemAnalysis* ActiveAnalysisObserver::getActiveObject() const
{
    return activeObject;
}

bool ActiveAnalysisObserver::hasActiveObject() const
{
    return activeObject != nullptr;
}

void ActiveAnalysisObserver::highlightActiveObject(const Gui::HighlightMode& mode, bool on)
{
    if (activeDocument && activeView) {
        activeDocument->signalHighlightObject(*activeView, mode, on, 0, 0);
    }
}

void ActiveAnalysisObserver::slotDeletedDocument(const App::Document& Doc)
{
    App::Document* d = getDocument();
    if (d == &Doc) {
        activeObject = nullptr;
        activeDocument = nullptr;
        activeView = nullptr;
        detachDocument();
    }
}

void ActiveAnalysisObserver::slotDeletedObject(const App::DocumentObject& Obj)
{
    if (activeObject == &Obj) {
        activeObject = nullptr;
        activeView = nullptr;
    }
}
