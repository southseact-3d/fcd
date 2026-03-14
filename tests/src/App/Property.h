#ifndef TEST_PROPERTY_H
#define TEST_PROPERTY_H

#include <gtest/gtest.h>

#include <App/Document.h>
#include <App/PropertyStandard.h>
#include <App/VarSet.h>

#include <src/App/InitApplication.h>

class RenameProperty: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
        _docName = App::GetApplication().getUniqueDocumentName("test");
        _doc = App::GetApplication().newDocument(_docName.c_str(), "testUser");
    }

    void SetUp() override
    {
        varSet = freecad_cast<App::VarSet*>(_doc->addObject("App::VarSet", "VarSet"));
        prop = freecad_cast<App::PropertyInteger*>(
            varSet->addDynamicProperty("App::PropertyInteger", "Variable", "Variables")
        );
        prop->setValue(Value);
    }

    void TearDown() override
    {
        _doc->removeObject(varSet->getNameInDocument());
    }

    static void TearDownTestSuite()
    {
        App::GetApplication().closeDocument(_docName.c_str());
    }

    const long Value = 123;
    App::VarSet* varSet {};
    App::PropertyInteger* prop {};

    static std::string _docName;
    static App::Document* _doc;
};

#endif
