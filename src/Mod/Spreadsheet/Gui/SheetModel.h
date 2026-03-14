#ifndef SHEETMODEL_H
#define SHEETMODEL_H

#include <QAbstractTableModel>

#include <App/Range.h>


namespace Spreadsheet
{
class Sheet;
}

namespace SpreadsheetGui
{

class SheetModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SheetModel(Spreadsheet::Sheet* _sheet, QObject* parent = nullptr);
    ~SheetModel() override;

    explicit SheetModel(QObject* parent);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex&) const override;

private Q_SLOTS:
    void setCellData(QModelIndex index, QString str);

private:
    void cellUpdated(App::CellAddress address);
    void rangeUpdated(const App::Range& range);

    fastsignals::scoped_connection cellUpdatedConnection;
    fastsignals::scoped_connection rangeUpdatedConnection;
    Spreadsheet::Sheet* sheet;
    QColor aliasBgColor;
    QColor textFgColor;
    QColor positiveFgColor;
    QColor negativeFgColor;

    QVariantList columnLabels, rowLabels;

    static constexpr int maxRowCount = 16384, maxColumnCount = 26 + 26 * 26;
};

}  // namespace SpreadsheetGui

#endif  // SHEETMODEL_H
