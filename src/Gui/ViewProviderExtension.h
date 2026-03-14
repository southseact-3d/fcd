#ifndef GUI_VIEWPROVIDEREXTENSION_H
#define GUI_VIEWPROVIDEREXTENSION_H

#include <QIcon>
#include <App/Extension.h>


class SoDetail;
class SoFullPath;
class SoGroup;
class SoPickedPoint;
class SoSeparator;

class QMenu;
class QObject;

namespace Gui
{

class ViewProvider;
class ViewProviderDocumentObject;

/**
 * @brief Extension with special viewprovider calls
 *
 */
class GuiExport ViewProviderExtension: public App::Extension
{
    // The class does not have properties itself, but it is important to provide the property access
    // functions.
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderExtension);

public:
    ViewProviderExtension();
    ~ViewProviderExtension() override;

    Gui::ViewProviderDocumentObject* getExtendedViewProvider();
    const Gui::ViewProviderDocumentObject* getExtendedViewProvider() const;

    virtual std::vector<App::DocumentObject*> extensionClaimChildren3D() const
    {
        return {};
    }

    virtual bool extensionOnDelete(const std::vector<std::string>&)
    {
        return true;
    }
    virtual void extensionBeforeDelete()
    {}

    virtual std::vector<App::DocumentObject*> extensionClaimChildren() const
    {
        return {};
    }

    virtual bool extensionCanDragObjects() const
    {
        return false;
    }
    virtual bool extensionCanDragObject(App::DocumentObject*) const
    {
        return true;
    }
    virtual void extensionDragObject(App::DocumentObject*)
    {}
    virtual bool extensionCanDropObjects() const
    {
        return false;
    }
    virtual bool extensionCanDropObject(App::DocumentObject*) const
    {
        return true;
    }
    virtual bool extensionCanDragAndDropObject(App::DocumentObject*) const
    {
        return true;
    }
    virtual void extensionDropObject(App::DocumentObject*)
    {}
    virtual bool extensionCanDropObjectEx(
        App::DocumentObject*,
        App::DocumentObject*,
        const char*,
        const std::vector<std::string>&
    ) const
    {
        return false;
    }
    virtual std::string extensionDropObjectEx(
        App::DocumentObject* obj,
        App::DocumentObject*,
        const char*,
        const std::vector<std::string>&
    )
    {
        extensionDropObject(obj);
        return {};
    }

    virtual int extensionReplaceObject(App::DocumentObject* /*oldValue*/, App::DocumentObject* /*newValue*/)
    {
        return -1;
    }

    /// Hides the view provider
    virtual void extensionHide()
    {}
    /// Shows the view provider
    virtual void extensionShow()
    {}

    virtual void extensionModeSwitchChange()
    {}

    virtual SoSeparator* extensionGetFrontRoot() const
    {
        return nullptr;
    }
    virtual SoGroup* extensionGetChildRoot() const
    {
        return nullptr;
    }
    virtual SoSeparator* extensionGetBackRoot() const
    {
        return nullptr;
    }
    virtual void extensionAttach(App::DocumentObject*)
    {}
    virtual void extensionReattach(App::DocumentObject*)
    {}
    virtual void extensionSetDisplayMode(const char*)
    {}
    virtual std::vector<std::string> extensionGetDisplayModes() const
    {
        return {};
    }
    virtual void extensionSetupContextMenu(QMenu*, QObject*, const char*)
    {}

    // update data of extended object
    virtual void extensionUpdateData(const App::Property*);
    PyObject* getExtensionPyObject() override;

    void setIgnoreOverlayIcon(bool on)
    {
        m_ignoreOverlayIcon = on;
    }
    bool ignoreOverlayIcon() const
    {
        return m_ignoreOverlayIcon;
    }
    virtual QIcon extensionMergeGreyableOverlayIcons(const QIcon& orig) const
    {
        return orig;
    }
    virtual QIcon extensionMergeColorfullOverlayIcons(const QIcon& orig) const
    {
        return orig;
    }

    virtual void extensionStartRestoring()
    {}
    virtual void extensionFinishRestoring()
    {}

    virtual bool extensionGetElementPicked(const SoPickedPoint*, std::string&) const
    {
        return false;
    }
    virtual bool extensionGetDetailPath(const char*, SoFullPath*, SoDetail*&) const
    {
        return false;
    }

private:
    bool m_ignoreOverlayIcon = false;
};

}  // namespace Gui

#endif  // GUI_VIEWPROVIDEREXTENSION_H
