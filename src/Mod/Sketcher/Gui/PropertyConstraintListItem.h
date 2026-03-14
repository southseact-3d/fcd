#ifndef PROPERTYCONSTRAINTLISTITEM_H
#define PROPERTYCONSTRAINTLISTITEM_H

#include <QObject>
#include <vector>

#include <Gui/propertyeditor/PropertyItem.h>


namespace SketcherGui
{

using FrameOption = Gui::PropertyEditor::FrameOption;

class PropertyConstraintListItem: public Gui::PropertyEditor::PropertyItem
{
    Q_OBJECT
    PROPERTYITEM_HEADER

    ~PropertyConstraintListItem() override;
    void assignProperty(const App::Property* prop) override;
    QWidget* createEditor(
        QWidget* parent,
        const std::function<void()>& method,
        FrameOption frameOption
    ) const override;
    void setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

protected:
    QString toString(const QVariant&) const override;
    QVariant value(const App::Property*) const override;
    void setValue(const QVariant&) override;
    bool event(QEvent* ev) override;

    void initialize() override;

protected:
    PropertyConstraintListItem();
    bool blockEvent;
    bool onlyUnnamed;
};

}  // namespace SketcherGui


#endif
