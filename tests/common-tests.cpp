#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>

class CommonTests : public ::testing::Test {

public:

    CommonTests() {
    }

    ~CommonTests() {
    }

    void SetUp() {

    }

    void TearDown() {
    }
};

TEST_F(CommonTests, Dummy) {
    ASSERT_EQ(true, true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}