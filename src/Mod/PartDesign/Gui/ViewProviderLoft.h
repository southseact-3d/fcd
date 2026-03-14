#ifndef PARTGUI_ViewProviderLoft_H
#define PARTGUI_ViewProviderLoft_H

#include "ViewProvider.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderLoft: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderLoft);

public:
    enum Reference
    {
        Profile,
        Section,
        Both
    };

    /// constructor
    ViewProviderLoft();
    /// destructor
    ~ViewProviderLoft() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    void highlightProfile(bool on);
    void highlightSection(bool on);
    void highlightReferences(Reference mode, bool on);

protected:
    QIcon getIcon() const override;
    TaskDlgFeatureParameters* getEditDialog() override;

private:
    void highlightReferences(Part::Feature*, const std::vector<std::string>&, bool);

private:
    std::map<long, std::vector<Base::Color>> originalLineColors;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderLoft_H
