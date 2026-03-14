#include <gtest/gtest.h>

#include <Mod/Spreadsheet/App/Sheet.h>

#include "src/App/Property.h"

class SpreadsheetRenameProperty: public RenameProperty
{
protected:
    void SetUp() override
    {
        RenameProperty::SetUp();
        sheet = freecad_cast<Spreadsheet::Sheet*>(_doc->addObject("Spreadsheet::Sheet", "Sheet"));
    }

    void TearDown() override
    {
        _doc->removeObject(sheet->getNameInDocument());
        RenameProperty::TearDown();
    }

    Spreadsheet::Sheet* sheet {};
};

std::string RenameProperty::_docName;
App::Document* RenameProperty::_doc {nullptr};

// Tests whether we can rename a property that is used in a spreadsheet
TEST_F(SpreadsheetRenameProperty, renameProperty)
{
    // Arrange
    sheet->setCell("A1", "=VarSet.Variable");
    _doc->recompute();
    auto* propSpreadsheet = static_cast<App::PropertyInteger*>(sheet->getPropertyByName("A1"));

    // Assert before the rename
    EXPECT_EQ(prop->getValue(), Value);
    EXPECT_EQ(propSpreadsheet->getValue(), Value);

    // Act
    bool isRenamed = varSet->renameDynamicProperty(prop, "NewName");
    _doc->recompute();

    // Assert after the rename
    EXPECT_TRUE(isRenamed);
    EXPECT_STREQ(varSet->getPropertyName(prop), "NewName");
    EXPECT_EQ(prop->getValue(), Value);

    EXPECT_EQ(varSet->getDynamicPropertyByName("Variable"), nullptr);
    EXPECT_EQ(varSet->getDynamicPropertyByName("NewName"), prop);

    std::string cellContent;
    sheet->getCell(App::CellAddress("A1"))->getStringContent(cellContent);

    EXPECT_EQ(cellContent, "=VarSet.NewName");
    EXPECT_EQ(propSpreadsheet->getValue(), Value);
}
