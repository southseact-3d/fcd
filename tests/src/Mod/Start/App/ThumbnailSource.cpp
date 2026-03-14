#include <gtest/gtest.h>
#include "src/App/InitApplication.h"

#include <memory>

#include <Mod/Start/App/ThumbnailSource.h>

class ThumbnailSourceTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {}

    void TearDown() override
    {}


private:
};
