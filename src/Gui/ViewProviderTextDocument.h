#ifndef GUI_ViewProviderTextDocument_H
#define GUI_ViewProviderTextDocument_H

#include <QPointer>
#include "ViewProviderDocumentObject.h"


class QPlainTextEdit;

namespace Gui
{

class GuiExport ViewProviderTextDocument: public ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderTextDocument);

public:
    ViewProviderTextDocument();
    ~ViewProviderTextDocument() override = default;

    App::PropertyBool ReadOnly;
    App::PropertyFloat FontSize;
    App::PropertyFont FontName;
    App::PropertyEnumeration SyntaxHighlighter;

    bool doubleClicked() override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool isShow() const override
    {
        return true;
    }

    void onChanged(const App::Property* prop) override;

    MDIView* getMDIView() const override;

private:
    bool activateView() const;

private:
    QPointer<QPlainTextEdit> editorWidget;
    static const char* SyntaxEnums[];
};

}  // namespace Gui

#endif
