#ifndef GUI_ActiveObjectList_H
#define GUI_ActiveObjectList_H

#include <map>
#include <string>
#include <Base/Type.h>
#include <Gui/TreeItemMode.h>
#include <FCGlobal.h>


namespace App
{
class DocumentObject;
}

namespace Gui
{
class Document;
class ViewProviderDocumentObject;

/** List of active or special objects
 * This class holds a list of objects with a special name.
 * Its mainly used to points to something like the active Body or Part in a edit session.
 * The class is used the viewer (editor) of a document.
 * @see Gui::MDIViewer
 * @author Jürgen Riegel
 */
class GuiExport ActiveObjectList
{
public:
    explicit ActiveObjectList(Document* doc)
        : _Doc(doc)
    {}

    template<typename _T>
    inline _T getObject(
        const char* name,
        App::DocumentObject** parent = nullptr,
        std::string* subname = nullptr
    ) const
    {
        auto it = _ObjectMap.find(name);
        if (it == _ObjectMap.end()) {
            return 0;
        }
        return dynamic_cast<_T>(getObject(it->second, true, parent, subname));
    }
    void setObject(
        App::DocumentObject*,
        const char*,
        const char* subname = nullptr,
        const Gui::HighlightMode& m = HighlightMode::UserDefined
    );
    bool hasObject(const char*) const;
    void objectDeleted(const ViewProviderDocumentObject& viewProviderIn);
    bool hasObject(App::DocumentObject* obj, const char*, const char* subname = nullptr) const;

    App::DocumentObject* getObjectWithExtension(Base::Type extensionTypeId) const;

private:
    struct ObjectInfo;
    void setHighlight(const ObjectInfo& info, Gui::HighlightMode mode, bool enable);
    App::DocumentObject* getObject(
        const ObjectInfo& info,
        bool resolve,
        App::DocumentObject** parent = nullptr,
        std::string* subname = nullptr
    ) const;
    ObjectInfo getObjectInfo(App::DocumentObject* obj, const char* subname) const;

private:
    struct ObjectInfo
    {
        App::DocumentObject* obj;
        std::string subname;
    };
    std::map<std::string, ObjectInfo> _ObjectMap;
    Document* _Doc;
};

}  // namespace Gui

static const char PDBODYKEY[] = "pdbody";
static const char PARTKEY[] = "part";
static const char ASSEMBLYKEY[] = "assembly";

#endif
