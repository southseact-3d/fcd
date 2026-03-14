#ifndef SPREADSHEET_ViewProviderImagePlane_H
#define SPREADSHEET_ViewProviderImagePlane_H

#include <QPointer>

#include <Gui/ViewProviderDocumentObject.h>
#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Spreadsheet/SpreadsheetGlobal.h>
#include <Mod/Spreadsheet/Gui/SpreadsheetView.h>


namespace SpreadsheetGui
{

class SpreadsheetGuiExport ViewProviderSheet: public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(SpreadsheetGui::ViewProviderSheet);

public:
    /// constructor.
    ViewProviderSheet();

    /// destructor.
    ~ViewProviderSheet() override;

    bool useNewSelectionModel() const override
    {
        return false;
    }

    bool doubleClicked() override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

    Spreadsheet::Sheet* getSpreadsheetObject() const;

    void beforeDelete() override;

    QIcon getIcon() const override;

    bool setEdit(int ModNum) override;

    bool isShow() const override
    {
        return true;
    }

    Gui::MDIView* getMDIView() const override;

    inline SheetView* getView() const
    {
        return view;
    }

    PyObject* getPyObject() override;

    void showSheetMdi();

    void exportAsFile();

protected:
    SheetView* showSpreadsheetView();
    void updateData(const App::Property* prop) override;

private:
    QPointer<SheetView> view;
};

using ViewProviderSheetPython = Gui::ViewProviderFeaturePythonT<ViewProviderSheet>;

}  // namespace SpreadsheetGui


#endif  // SPREADSHEET_ViewProviderSpreadsheet_H
