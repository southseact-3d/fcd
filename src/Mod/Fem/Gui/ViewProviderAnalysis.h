#ifndef FEM_ViewProviderAnalysis_H
#define FEM_ViewProviderAnalysis_H

#include <Gui/ViewProviderDocumentObjectGroup.h>
#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Fem/FemGlobal.h>
#include <QCoreApplication>

namespace FemGui
{

class ViewProviderFemAnalysis;
class ViewProviderFemHighlighter
{
public:
    /// Constructor
    ViewProviderFemHighlighter();
    ~ViewProviderFemHighlighter();

    void attach(ViewProviderFemAnalysis*);
    void highlightView(Gui::ViewProviderDocumentObject*);
    void removeView(Gui::ViewProviderDocumentObject*);

private:
    SoSeparator* annotate;
};

class FemGuiExport ViewProviderFemAnalysis: public Gui::ViewProviderDocumentObjectGroup
{
    Q_DECLARE_TR_FUNCTIONS(FemGui::ViewProviderFemAnalysis)
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemAnalysis);

public:
    /// constructor
    ViewProviderFemAnalysis();

    /// destructor
    ~ViewProviderFemAnalysis() override;

    void attach(App::DocumentObject*) override;
    bool doubleClicked() override;

    std::vector<App::DocumentObject*> claimChildren() const override;

    /// handling when object is deleted
    bool onDelete(const std::vector<std::string>&) override;
    /// warning on deletion when there are children
    static bool checkSelectedChildren(
        const std::vector<App::DocumentObject*> objs,
        Gui::Document* docGui,
        std::string objectName
    );
    /// asks the view provider if the given object can be deleted
    bool canDelete(App::DocumentObject* obj) const override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

    /// shows solid in the tree
    bool isShow() const override
    {
        return Visibility.getValue();
    }
    /// Hide the object in the view
    void hide() override;
    /// Show the object in the view
    void show() override;

    void highlightView(Gui::ViewProviderDocumentObject*);

    void removeView(Gui::ViewProviderDocumentObject*);

    /** @name Drag and drop */
    //@{
    /// Returns true if the view provider generally supports dragging objects
    bool canDragObjects() const override;
    /// Check whether the object can be removed from the view provider by drag and drop
    bool canDragObject(App::DocumentObject*) const override;
    /// Starts to drag the object
    void dragObject(App::DocumentObject*) override;
    /// Returns true if the view provider generally accepts dropping of objects
    bool canDropObjects() const override;
    /// Check whether the object can be dropped to the view provider by drag and drop
    bool canDropObject(App::DocumentObject*) const override;
    /// If the dropped object type is accepted the object will be added as child
    void dropObject(App::DocumentObject*) override;
    //@}

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;

private:
    ViewProviderFemHighlighter extension;
};

using ViewProviderFemAnalysisPython = Gui::ViewProviderFeaturePythonT<ViewProviderFemAnalysis>;

}  // namespace FemGui


#endif  // FEM_ViewProviderAnalysis_H
