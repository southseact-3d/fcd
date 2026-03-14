#ifndef TECHDRAWGUI_VIEWPROVIDERVIEW_H
#define TECHDRAWGUI_VIEWPROVIDERVIEW_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <fastsignals/signal.h>

#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/TechDraw/App/DrawView.h>

#include "ViewProviderDrawingViewExtension.h"


namespace TechDraw {
class DrawView;
}

namespace TechDrawGui {
class QGIView;
class MDIViewPage;
class ViewProviderPage;

class TechDrawGuiExport ViewProviderDrawingView : public Gui::ViewProviderDocumentObject,
                                                  public ViewProviderDrawingViewExtension
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderDrawingView);

public:
    /// constructor
    ViewProviderDrawingView();
    /// destructor
    ~ViewProviderDrawingView() override;

    App::PropertyBool  KeepLabel;
    App::PropertyInteger StackOrder;

    void attach(App::DocumentObject *) override;
    bool useNewSelectionModel() const override {return false;}
    /// Hide the object in the view
    void hide() override;
    /// Show the object in the view
    void show() override;
    bool isShow() const override;

    void dropObject(App::DocumentObject* docObj) override;

    void onChanged(const App::Property *prop) override;
    void updateData(const App::Property*) override;

    QGIView* getQView();
    MDIViewPage* getMDIViewPage() const;
    Gui::MDIView *getMDIView() const override;
    ViewProviderPage* getViewProviderPage() const;

    /** @name Restoring view provider from document load */
    //@{
    void startRestoring() override;
    void finishRestoring() override;
    //@}

    virtual TechDraw::DrawView* getViewObject() const;
    void showProgressMessage(const std::string featureName, const std::string text) const;

    void onGuiRepaint(const TechDraw::DrawView* dv);
    void onProgressMessage(const TechDraw::DrawView* dv,
                         const std::string featureName,
                         const std::string text);
    using Connection = fastsignals::scoped_connection;
    Connection connectGuiRepaint;
    Connection connectProgressMessage;

    virtual void stackUp();
    virtual void stackDown();
    virtual void stackTop();
    virtual void stackBottom();
    virtual int getZ() {return StackOrder.getValue();}

    const char* whoAmI() const;

    virtual void fixSceneDependencies();
    std::vector<App::DocumentObject*> claimChildren() const override;

    void fixColorAlphaValues();
    bool checkMiniumumDocumentVersion(int minMajor, int minMinor) const
        { return checkMiniumumDocumentVersion(this->getDocument()->getDocument(), minMajor, minMinor); }
    static bool checkMiniumumDocumentVersion(App::Document* toBeChecked, int minMajor, int minMinor);


private:
    void multiParentPaint(std::vector<TechDraw::DrawPage*>& pages);
    void singleParentPaint(const TechDraw::DrawView* dv);

    std::string m_myName;
};

} // namespace TechDrawGui


#endif // TECHDRAWGUI_VIEWPROVIDERVIEW_H
