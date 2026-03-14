#ifndef PARTGUI_ViewProviderTransformed_H
#define PARTGUI_ViewProviderTransformed_H

#include "ViewProvider.h"

#include <Inventor/nodes/SoMultipleCopy.h>

namespace PartDesign
{
class Transformed;
}

namespace PartDesignGui
{

class TaskDlgTransformedParameters;

class PartDesignGuiExport ViewProviderTransformed: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderTransformed);

public:
    ViewProviderTransformed() = default;
    ~ViewProviderTransformed() override = default;

    // The feature name of the subclass
    virtual const std::string& featureName() const;
    std::string featureIcon() const;

    void recomputeFeature(bool recompute = true);
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    /// signals if the transformation contains errors
    fastsignals::signal<void(QString msg)> signalDiagnosis;

    // Name of menu dialog
    QString menuName;

    Gui::ViewProvider* startEditing(int ModNum = 0) override;

    QString getMessage() const
    {
        return diagMessage;
    }

protected:
    bool setEdit(int ModNum) override;

    void attachPreview() override;
    void updatePreview() override;

    bool checkDlgOpen(TaskDlgTransformedParameters* transformedDlg);
    void handleTransformedResult(PartDesign::Transformed* transformed);

    Gui::CoinPtr<SoMultipleCopy> pcMultipleCopy;
    QString diagMessage;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderTransformed_H
