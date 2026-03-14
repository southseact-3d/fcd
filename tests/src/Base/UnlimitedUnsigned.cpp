#include <gtest/gtest.h>
#include <string>
#include <Base/UnlimitedUnsigned.h>

// NOLINTBEGIN(cppcoreguidelines-*,readability-*)
TEST(UnlimitedUnsigned, Basics)
{
    // Check simple addition with carry and conversion from string
    Base::UnlimitedUnsigned one(1);
    auto nines = Base::UnlimitedUnsigned::fromString("999999999");
    EXPECT_EQ(nines + one, Base::UnlimitedUnsigned::fromString("1000000000"));
}
TEST(UnlimitedUnsigned, ToString)
{
    // Check toString on simple addition result
    Base::UnlimitedUnsigned one(1);
    auto nines = Base::UnlimitedUnsigned::fromString("999999999");
    EXPECT_EQ((nines + one).toString(), "1000000000");
}
TEST(UnlimitedUnsigned, TestSubtraction1)
{
    // Check subtraction and comparison with byte-sized number
    EXPECT_EQ(
        Base::UnlimitedUnsigned::fromString("6842357951")
            - Base::UnlimitedUnsigned::fromString("6842357948"),
        3
    );
}
TEST(UnlimitedUnsigned, TestSubtraction2)
{
    // Check subtraction and comparison
    EXPECT_EQ(
        Base::UnlimitedUnsigned::fromString("6842357951")
            - Base::UnlimitedUnsigned::fromString("6000000000"),
        Base::UnlimitedUnsigned::fromString("842357951")
    );
}

// NOLINTEND(cppcoreguidelines-*,readability-*)
