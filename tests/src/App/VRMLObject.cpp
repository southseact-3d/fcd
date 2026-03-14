#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include <App/Application.h>
#include <App/Document.h>
#include <App/VRMLObject.h>
#include <Base/FileInfo.h>
#include <src/App/InitApplication.h>

// NOLINTBEGIN

class VRMLObjectTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        document = App::GetApplication().openDocument(fileName().c_str());
    }

    void TearDown() override
    {
        if (document) {
            App::GetApplication().closeDocument(document->getName());
        }
    }

    std::string fileName() const
    {
        std::string resDir(DATADIR);
        resDir.append("/tests/TestVRMLTextures.FCStd");
        return resDir;
    }

    App::Document* getDocument() const
    {
        return document;
    }

private:
    App::Document* document {};
};

TEST_F(VRMLObjectTest, loadVRMLWithTextures)
{
    App::Document* doc = getDocument();
    ASSERT_TRUE(doc);

    auto vrml = dynamic_cast<App::VRMLObject*>(doc->getActiveObject());
    ASSERT_TRUE(vrml);

    auto res = vrml->Resources.getValues();
    EXPECT_EQ(res.size(), 6);
    EXPECT_EQ(res[0], std::string("FreeCAD/FreeCAD1.png"));
    EXPECT_EQ(res[1], std::string("FreeCAD/FreeCAD2.png"));
    EXPECT_EQ(res[2], std::string("FreeCAD/FreeCAD3.png"));
    EXPECT_EQ(res[3], std::string("FreeCAD/FreeCAD4.png"));
    EXPECT_EQ(res[4], std::string("FreeCAD/FreeCAD5.png"));
    EXPECT_EQ(res[5], std::string("FreeCAD/FreeCAD6.png"));

    auto url = vrml->Urls.getValues();
    EXPECT_EQ(url.size(), 6);
    for (const auto& it : url) {
        Base::FileInfo fi(it);
        EXPECT_TRUE(fi.isFile());
        EXPECT_TRUE(fi.exists());
    }
}

// NOLINTEND
