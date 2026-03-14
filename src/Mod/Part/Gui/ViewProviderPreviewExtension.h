#ifndef PARTGUI_VIEWPROVIDERPREVIEWEXTENSION_H
#define PARTGUI_VIEWPROVIDERPREVIEWEXTENSION_H

#include "SoBrepEdgeSet.h"
#include "SoBrepFaceSet.h"
#include "SoBrepPointSet.h"
#include "SoFCShapeObject.h"

#include <QtCore>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFMatrix.h>

#include <App/PropertyStandard.h>
#include <Gui/ViewProvider.h>

#include <Gui/ViewProviderExtension.h>
#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/PartGlobal.h>

namespace PartGui
{

class PartGuiExport SoPreviewShape: public SoFCShape
{
    using inherited = SoFCShape;
    SO_NODE_HEADER(SoPreviewShape);

public:
    static constexpr float defaultTransparency = 0.8F;
    static constexpr float defaultLineWidth = 2.0F;
    static const SbColor defaultColor;

    SoPreviewShape();
    static void initClass();

    SoSFColor color;
    SoSFFloat transparency;
    SoSFFloat lineWidth;
    SoSFMatrix transform;

private:
    SoMatrixTransform* pcTransform;
};

class PartGuiExport ViewProviderPreviewExtension: public Gui::ViewProviderExtension
{
    Q_DECLARE_TR_FUNCTIONS(PartGui::ViewProviderPreviewExtension)
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderPreviewExtension);

public:
    App::PropertyColor PreviewColor;

    ViewProviderPreviewExtension();

    /// Returns shape that should be used as the preview
    virtual Part::TopoShape getPreviewShape() const
    {
        return Part::TopoShape();
    };

    void extensionAttach(App::DocumentObject*) override;
    void extensionBeforeDelete() override;

    /// Returns whatever preview is enabled or not
    bool isPreviewEnabled() const
    {
        return _isPreviewEnabled;
    }
    /// Switches preview on or off
    virtual void showPreview(bool enable);

protected:
    void extensionOnChanged(const App::Property* prop) override;

    /// attaches preview to the scene graph
    virtual void attachPreview();
    /// updates preview
    virtual void updatePreview();
    /// updates geometry of the preview shape
    void updatePreviewShape(Part::TopoShape shape, SoPreviewShape* preview);

    Gui::CoinPtr<SoSeparator> pcPreviewRoot;
    Gui::CoinPtr<SoPreviewShape> pcPreviewShape;

private:
    bool _isPreviewEnabled {false};
};

using ViewProviderPreviewExtensionPython
    = Gui::ViewProviderExtensionPythonT<ViewProviderPreviewExtension>;

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDERPREVIEWEXTENSION_H
