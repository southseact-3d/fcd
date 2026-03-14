#ifndef PART_PropertyEnumAttacherItem_H
#define PART_PropertyEnumAttacherItem_H

#include <Gui/propertyeditor/PropertyItem.h>

#include <Mod/Part/PartGlobal.h>

#include "TaskAttacher.h"

namespace PartGui
{

using FrameOption = Gui::PropertyEditor::FrameOption;

/**
 * Custom editor item for PropertyEnumeration to open Attacher task
 */
class PartGuiExport PropertyEnumAttacherItem: public Gui::PropertyEditor::PropertyEnumItem
{
    Q_OBJECT

public:
    PROPERTYITEM_HEADER

    QWidget* createEditor(
        QWidget* parent,
        const std::function<void()>& method,
        FrameOption frameOption
    ) const override;
    void setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

protected Q_SLOTS:
    void openTask();

protected:
    PropertyEnumAttacherItem();
};

}  // namespace PartGui

#endif  // PART_PropertyEnumAttacherItem_H
