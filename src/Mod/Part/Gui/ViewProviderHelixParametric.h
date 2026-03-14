#ifndef PARTGUI_VIEWPROVIDERHELIXPARAMETRIC_H
#define PARTGUI_VIEWPROVIDERHELIXPARAMETRIC_H

#include "ViewProviderSpline.h"
#include "ViewProviderPrimitive.h"

#include <Mod/Part/PartGlobal.h>

namespace PartGui
{


class PartGuiExport ViewProviderHelixParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderHelixParametric);

public:
    /// constructor
    ViewProviderHelixParametric();
    /// destructor
    ~ViewProviderHelixParametric() override;
    std::vector<std::string> getDisplayModes() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

private:
    ViewProviderSplineExtension extension;
};

class PartGuiExport ViewProviderSpiralParametric: public ViewProviderPrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderSpiralParametric);

public:
    /// constructor
    ViewProviderSpiralParametric();
    /// destructor
    ~ViewProviderSpiralParametric() override;
    std::vector<std::string> getDisplayModes() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

private:
    ViewProviderSplineExtension extension;
};

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERHELIXPARAMETRIC_H
