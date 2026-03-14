#ifndef GUI_VIEWPROVIDER_ViewProviderOrigin_H
#define GUI_VIEWPROVIDER_ViewProviderOrigin_H

#include <functional>
#include <Base/Bitmask.h>
#include <App/PropertyGeo.h>

#include "ViewProviderGeoFeatureGroup.h"


namespace Gui
{

class Document;
class ViewProviderDatum;

enum class DatumElement
{
    // clang-format off
    Origin = 1 << 0,
    Axes   = 1 << 1,
    Planes = 1 << 2
    // clang-format on
};

using DatumElements = Base::Flags<DatumElement>;

class GuiExport ViewProviderCoordinateSystem: public ViewProviderGeoFeatureGroup
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderCoordinateSystem);

public:
    /// constructor.
    ViewProviderCoordinateSystem();
    /// destructor.
    ~ViewProviderCoordinateSystem() override;

    /// @name Override methods
    ///@{
    std::vector<App::DocumentObject*> claimChildren() const override;
    std::vector<App::DocumentObject*> claimChildren3D() const override;

    SoGroup* getChildRoot() const override
    {
        return pcGroupChildren;
    }

    void attach(App::DocumentObject* pcObject) override;
    std::vector<std::string> getDisplayModes() const override;
    void setDisplayMode(const char* ModeName) override;
    ///@}

    /** @name Temporary visibility mode
     * Control the visibility of origin and associated objects when needed
     */
    ///@{
    /// Set temporary visibility of some of origin's objects e.g. while rotating or mirroring
    void setTemporaryVisibility(DatumElements elements);
    /// Returns true if the origin in temporary visibility mode
    bool isTemporaryVisibility();
    /// Reset the visibility
    void resetTemporaryVisibility();
    ///@}

    void setTemporaryScale(double factor);
    void resetTemporarySize();

    void setPlaneLabelVisibility(bool val);

    bool canDragObjects() const override
    {
        return false;
    }

    /// Returns default size. Use this if it is not possible to determine appropriate size by other means
    static double defaultSize();

    // the factor by which the axes are longer than the planes
    static constexpr float axesScaling = 1.5f;

    // default color for origini: light-blue (50, 150, 250, 255 stored as 0xRRGGBBAA)
    static const uint32_t defaultColor = 0x3296faff;

protected:
    bool onDelete(const std::vector<std::string>&) override;

private:
    using DatumObjectFunc = std::function<void(ViewProviderDatum*)>;
    void applyDatumObjects(const DatumObjectFunc& func);

private:
    SoGroup* pcGroupChildren;

    std::map<Gui::ViewProvider*, bool> tempVisMap;
};

}  // namespace Gui

ENABLE_BITMASK_OPERATORS(Gui::DatumElement)

#endif  // GUI_VIEWPROVIDER_ViewProviderOrigin_H
