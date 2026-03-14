#ifndef IMPORT_OCAFBROWSER_H
#define IMPORT_OCAFBROWSER_H

#include <QIcon>
#include <TCollection_ExtendedString.hxx>
#include <TDF_IDList.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_NamedShape.hxx>
#include <string>

class QString;
class QTreeWidget;
class QTreeWidgetItem;

namespace ImportGui
{
class OCAFBrowser
{
public:
    explicit OCAFBrowser(const Handle(TDocStd_Document) & hDoc);
    void load(QTreeWidget*);

    static void showDialog(const QString& title, const Handle(TDocStd_Document) & hDoc);

private:
    void load(const TDF_Label& label, QTreeWidgetItem* item, const QString&);
    static std::string toString(const TCollection_ExtendedString& extstr);
    static QString toText(const Handle(TDataStd_TreeNode) & treeNode);
    static QString toText(const Handle(TNaming_NamedShape) & namedShape);

private:
    QIcon myGroupIcon;
    TDF_IDList myList;
    Handle(TDocStd_Document) pDoc;
};

}  // namespace ImportGui

#endif  // IMPORT_OCAFBROWSER_H
