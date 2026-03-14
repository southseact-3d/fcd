#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include <App/Application.h>
#include "App/Document.h"
#include <App/VarSet.h>
#include <src/App/InitApplication.h>

using ::testing::NotNull;

// NOLINTBEGIN(readability-magic-numbers)

class VarSet: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        _docName = App::GetApplication().getUniqueDocumentName("test");
        _doc = App::GetApplication().newDocument(_docName.c_str(), "testUser");
    }

    void TearDown() override
    {
        App::GetApplication().closeDocument(_docName.c_str());
    }

    App::Document* doc()
    {
        return _doc;
    }

private:
    std::string _docName;
    App::Document* _doc {};
};

// Tests whether we can create a VarSet
TEST_F(VarSet, createVarSet)
{
    // Arrange
    const char* nameVarSet = "VarSet";

    // Act
    doc()->addObject("App::VarSet", nameVarSet);
    auto varSet = dynamic_cast<App::VarSet*>(doc()->getObject(nameVarSet));

    // Assert
    EXPECT_THAT(varSet, NotNull());
}

// Tests whether we can add a property to a VarSet
TEST_F(VarSet, addProperty)
{
    // Arrange
    const char* nameVarSet = "VarSet001";
    const long VALUE = 123;

    doc()->addObject("App::VarSet", nameVarSet);
    auto varSet = dynamic_cast<App::VarSet*>(doc()->getObject(nameVarSet));

    // Act
    auto prop = dynamic_cast<App::PropertyInteger*>(
        varSet->addDynamicProperty("App::PropertyInteger", "Variable", "Variables")
    );
    prop->setValue(VALUE);

    // Assert
    EXPECT_EQ(prop->getValue(), VALUE);
}

// NOLINTEND(readability-magic-numbers)
