#ifndef PARTGUI_ViewProviderDressUp_H
#define PARTGUI_ViewProviderDressUp_H

#include "ViewProvider.h"


namespace PartDesignGui
{

class TaskDlgDressUpParameters;

class PartDesignGuiExport ViewProviderDressUp: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDressUp);

public:
    /// constructor
    ViewProviderDressUp() = default;
    /// destructor
    ~ViewProviderDressUp() override = default;

    void attach(App::DocumentObject* pcObject) override;

    /// grouping handling
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    /// Highlight the references that have been selected
    void highlightReferences(const bool on);

    /// Set preview parameters to indicate error state
    void setErrorState(bool error);

    /**
     * Returns the feature Name associated with the view provider.
     * Should be reimplemented in the successor.
     */
    virtual const std::string& featureName() const;
    std::string featureIcon() const;
    QString menuName;

protected:
    bool setEdit(int ModNum) override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDressUp_H
