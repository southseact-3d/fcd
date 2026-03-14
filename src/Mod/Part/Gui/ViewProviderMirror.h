#ifndef PARTGUI_VIEWPROVIDERMIRROR_H
#define PARTGUI_VIEWPROVIDERMIRROR_H

#include <Mod/Part/PartGlobal.h>

#include <Mod/Part/Gui/ViewProvider.h>

namespace PartGui
{


class PartGuiExport ViewProviderMirror: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderMirror);

public:
    ViewProviderMirror();
    ~ViewProviderMirror() override;
    /** @name Edit methods */
    //@{
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    //@}

private:
    static void dragStartCallback(void* data, SoDragger* d);
    static void dragFinishCallback(void* data, SoDragger* d);
    static void dragMotionCallback(void* data, SoDragger* d);

private:
    SoSeparator* pcEditNode;
};

class PartGuiExport ViewProviderFillet: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderFillet);

public:
    ViewProviderFillet();
    ~ViewProviderFillet() override;
    /** @name Edit methods */
    //@{
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    void updateData(const App::Property*) override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    //@}
};

class ViewProviderChamfer: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderChamfer);

public:
    /// constructor
    ViewProviderChamfer();
    /// destructor
    ~ViewProviderChamfer() override;
    /** @name Edit methods */
    //@{
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    void updateData(const App::Property*) override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    //@}
};

class ViewProviderRevolution: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderRevolution);

public:
    /// constructor
    ViewProviderRevolution();
    /// destructor
    ~ViewProviderRevolution() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;
};

class ViewProviderLoft: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderLoft);

public:
    /// constructor
    ViewProviderLoft();
    /// destructor
    ~ViewProviderLoft() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;
};

class ViewProviderSweep: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderSweep);

public:
    /// constructor
    ViewProviderSweep();
    /// destructor
    ~ViewProviderSweep() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    bool onDelete(const std::vector<std::string>&) override;
};

class ViewProviderOffset: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderOffset);

public:
    /// constructor
    ViewProviderOffset();
    /// destructor
    ~ViewProviderOffset() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

class ViewProviderOffset2D: public ViewProviderOffset
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderOffset2D);

public:
    ViewProviderOffset2D()
    {
        sPixmap = "Part_Offset2D";
    }
};

class ViewProviderThickness: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderThickness);

public:
    /// constructor
    ViewProviderThickness();
    /// destructor
    ~ViewProviderThickness() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool onDelete(const std::vector<std::string>&) override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

class ViewProviderRefine: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderRefine);

public:
    /// constructor
    ViewProviderRefine();
    /// destructor
    ~ViewProviderRefine() override;
};

class ViewProviderReverse: public ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderReverse);

public:
    /// constructor
    ViewProviderReverse();
    /// destructor
    ~ViewProviderReverse() override;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERMIRROR_H
