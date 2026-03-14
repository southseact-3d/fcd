#ifndef PARTGUI_ViewProviderPipe_H
#define PARTGUI_ViewProviderPipe_H

#include "ViewProvider.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderPipe: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderPipe);

public:
    enum Reference
    {
        Spine,
        AuxiliarySpine,
        Profile,
        Section
    };

    /// constructor
    ViewProviderPipe();
    /// destructor
    ~ViewProviderPipe() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    void highlightReferences(Reference mode, bool on);

protected:
    QIcon getIcon() const override;

    /// Returns a newly created TaskDlgPipeParameters
    TaskDlgFeatureParameters* getEditDialog() override;

private:
    void highlightReferences(Part::Feature*, const std::vector<std::string>&, bool);

private:
    std::map<long, std::vector<Base::Color>> originalLineColors;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderPipe_H
