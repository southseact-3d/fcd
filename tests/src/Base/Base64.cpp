#include "Base/Base64.h"

#include <gtest/gtest.h>

using namespace Base;

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

TEST(Base64, encode)
{
    const std::string str = "René Nyffenegger\n"
                            "http://www.renenyffenegger.ch\n"
                            "passion for data\n";

    auto encoded = base64_encode(reinterpret_cast<const unsigned char*>(str.c_str()), str.length());
    auto decoded = base64_decode(std::string(encoded));

    ASSERT_EQ(decoded, str);
}

TEST(Base64, exactlyFourBytes)
{
    // Test all possibilities of fill bytes (none, one =, two ==)
    // References calculated with: https://www.base64encode.org/

    std::string rest0_original = "abc";
    // std::string rest0_reference = "YWJj";

    std::string rest0_encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(rest0_original.c_str()),
        rest0_original.length()
    );
    std::string rest0_decoded = base64_decode(rest0_encoded);

    ASSERT_EQ(rest0_decoded, rest0_original);
}

TEST(Base64, twoEqualsSignsPadding)
{
    std::string rest1_original = "abcd";
    // std::string rest1_reference = "YWJjZA==";

    std::string rest1_encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(rest1_original.c_str()),
        rest1_original.length()
    );
    std::string rest1_decoded = base64_decode(rest1_encoded);

    ASSERT_EQ(rest1_decoded, rest1_original);
}

TEST(Base64, oneEqualsSignPadding)
{
    std::string rest2_original = "abcde";
    // std::string rest2_reference = "YWJjZGU=";

    std::string rest2_encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(rest2_original.c_str()),
        rest2_original.length()
    );
    std::string rest2_decoded = base64_decode(rest2_encoded);

    ASSERT_EQ(rest2_decoded, rest2_original);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
