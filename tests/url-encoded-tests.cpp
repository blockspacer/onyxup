#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>

#include "../sources/server/utils.h"

class UrlEncodedTests : public ::testing::Test {

public:

    UrlEncodedTests() {
    }

    ~UrlEncodedTests() {
    }

    void SetUp() {

    }

    void TearDown() {
    }

};

TEST_F(UrlEncodedTests, Test_1) {
    const char * uri = "id=1";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_STREQ(params["id"].c_str(), "1");
}
TEST_F(UrlEncodedTests, Test_2) {
    const char * uri = "id=1&param=2";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_STREQ(params["id"].c_str(), "1");
    ASSERT_STREQ(params["param"].c_str(), "2");
}
TEST_F(UrlEncodedTests, Test_3) {
    const char * uri = "";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}
TEST_F(UrlEncodedTests, Test_4) {
    const char * uri = "id";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}
TEST_F(UrlEncodedTests, Test_5) {
    const char * uri = "id=";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}
TEST_F(UrlEncodedTests, Test_6) {
    const char * uri = "&";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}
TEST_F(UrlEncodedTests, Test_7) {
    const char * uri = "&&&";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}
TEST_F(UrlEncodedTests, Test_8) {
    const char * uri = "=id";
    auto params = onyxup::utils::urlencoded(uri);
    ASSERT_EQ(params.size(), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
