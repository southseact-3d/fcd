#include "ViewProvider.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderPrimitive: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderPrimitive);

public:
    /// constructor
    ViewProviderPrimitive();
    /// destructor
    ~ViewProviderPrimitive() override;

protected:
    QIcon getIcon() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

    TaskDlgFeatureParameters* getEditDialog() override;

    std::string displayMode;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderBoolean_H
