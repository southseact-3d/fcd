#ifndef PARTGUI_ViewProviderDatum_H
#define PARTGUI_ViewProviderDatum_H

#include <Gui/ViewProviderLine.h>
#include <Gui/ViewProviderPlane.h>
#include <Gui/ViewProviderPoint.h>
#include <Gui/ViewProviderCoordinateSystem.h>
#include <QCoreApplication>

#include <Mod/Part/PartGlobal.h>

#include <Mod/Part/Gui/ViewProviderAttachExtension.h>

namespace PartGui
{

class PartGuiExport ViewProviderLine: public Gui::ViewProviderLine,
                                      PartGui::ViewProviderAttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartGui::ViewProviderLine);

public:
    ViewProviderLine();
    ~ViewProviderLine() override = default;

    bool doubleClicked() override;
};

class PartGuiExport ViewProviderPlane: public Gui::ViewProviderPlane,
                                       PartGui::ViewProviderAttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartGui::ViewProviderPlane);

public:
    ViewProviderPlane();
    ~ViewProviderPlane() override = default;

    bool doubleClicked() override;
};

class PartGuiExport ViewProviderPoint: public Gui::ViewProviderPoint,
                                       PartGui::ViewProviderAttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartGui::ViewProviderPoint);

public:
    ViewProviderPoint();
    ~ViewProviderPoint() override = default;

    bool doubleClicked() override;
};

class PartGuiExport ViewProviderLCS: public Gui::ViewProviderCoordinateSystem,
                                     PartGui::ViewProviderAttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartGui::ViewProviderLCS);

public:
    ViewProviderLCS();
    ~ViewProviderLCS() override = default;

    bool doubleClicked() override;
};

}  // namespace PartGui


#endif  // PARTGUI_ViewProviderDatum_H
