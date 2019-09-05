#include <gtest/gtest.h>
#include <vector>
#include <iostream>

#include "../sources/server/utils.h"

class ParseRangesRequestTests : public ::testing::Test {

public:

    ParseRangesRequestTests() {
    }

    ~ParseRangesRequestTests() {
    }

    void SetUp() {

    }

    void TearDown() {
    }

};

TEST_F(ParseRangesRequestTests, Test_1) {
    std::string src = "bytes=10-";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
}
TEST_F(ParseRangesRequestTests, Test_2) {
    std::string src = "bytes=103-";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
    ASSERT_EQ(ranges[0].first, 103);
    ASSERT_EQ(ranges[0].second,  20000);
}
TEST_F(ParseRangesRequestTests, Test_3) {
    std::string src = "bytes=-10";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
}
TEST_F(ParseRangesRequestTests, Test_4) {
    std::string src = "bytes=-103";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
    ASSERT_EQ(ranges[0].first, 20000 - 103);
    ASSERT_EQ(ranges[0].second,  20000);
}
TEST_F(ParseRangesRequestTests, Test_5) {
    std::string src = "bytes=10-100";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
}
TEST_F(ParseRangesRequestTests, Test_6) {
    std::string src = "bytes=256-1040";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 1);
    ASSERT_EQ(ranges[0].first, 256);
    ASSERT_EQ(ranges[0].second, 1040);
}
TEST_F(ParseRangesRequestTests, Test_7) {
    std::string src = "bytes=10-100, 120-140";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 2);
}
TEST_F(ParseRangesRequestTests, Test_8) {
    std::string src = "bytes=10-100, 120-140";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 2);
    ASSERT_EQ(ranges[0].first, 10);
    ASSERT_EQ(ranges[0].second, 100);
    ASSERT_EQ(ranges[1].first, 120);
    ASSERT_EQ(ranges[1].second, 140);
}

TEST_F(ParseRangesRequestTests, Test_9) {
    std::string src = "bytes=10-100, -140";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 2);
    ASSERT_EQ(ranges[0].first, 10);
    ASSERT_EQ(ranges[0].second, 100);
    ASSERT_EQ(ranges[1].first, 20000 - 140);
    ASSERT_EQ(ranges[1].second, 20000);
}

TEST_F(ParseRangesRequestTests, Test_10) {
    std::string src = "bytes=100 - 500, -140";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 2);
    ASSERT_EQ(ranges[0].first, 100);
    ASSERT_EQ(ranges[0].second, 500);
    ASSERT_EQ(ranges[1].first, 20000 - 140);
    ASSERT_EQ(ranges[1].second, 20000);
}

TEST_F(ParseRangesRequestTests, Test_11) {
    std::string src = "bytes=100-130, 140-";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges.size(), 2);
    ASSERT_EQ(ranges[0].first, 100);
    ASSERT_EQ(ranges[0].second, 130);
    ASSERT_EQ(ranges[1].first, 140);
    ASSERT_EQ(ranges[1].second, 20000);
}

TEST_F(ParseRangesRequestTests, Test_12) {
    std::string src = "bytes=";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_13) {
    std::string src = "bytes=100";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}
TEST_F(ParseRangesRequestTests, Test_14) {
    std::string src = "bytes=-100-";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}
TEST_F(ParseRangesRequestTests, Test_15) {
    std::string src = "bytes=-aaa";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}
TEST_F(ParseRangesRequestTests, Test_16) {
    std::string src = "bytes=200-100";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}
TEST_F(ParseRangesRequestTests, Test_17) {
    std::string src = "bytes=100-100";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges[0].first, 100);
    ASSERT_EQ(ranges[0].second, 100);
}

TEST_F(ParseRangesRequestTests, Test_18) {
    std::string src = "bytes=-30000";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}
TEST_F(ParseRangesRequestTests, Test_19) {
    std::string src = "bytes=100-200, 150-250";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_20) {
    std::string src = "bytes=-200, -500";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_21) {
    std::string src = "bytes=-20000";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges[0].first, 0);
    ASSERT_EQ(ranges[0].second, 20000);
}
TEST_F(ParseRangesRequestTests, Test_22) {
    std::string src = "bytes=0-";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges[0].first, 0);
    ASSERT_EQ(ranges[0].second, 20000);
}

TEST_F(ParseRangesRequestTests, Test_23) {
    std::string src = "bytes=100-200, 100-200";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_24) {
    std::string src = "bytes=0-20001";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_25) {
    std::string src = "bytes=0-0";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 1);
    ASSERT_EQ(ranges[0].first, 0);
    ASSERT_EQ(ranges[0].second, 0);
}

TEST_F(ParseRangesRequestTests, Test_26) {
    std::string src = "bytes=0-0";
    try{
        std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 0);
        ASSERT_EQ(true, false);
    } catch (onyxup::OnyxupException & ex){
        ASSERT_EQ(true, true);
    }
}

TEST_F(ParseRangesRequestTests, Test_27) {
    std::string src = "bytes=-0";
    std::vector<std::pair<size_t , size_t >> ranges = onyxup::utils::parseRangesRequest(src, 20000);
    ASSERT_EQ(ranges[0].first, 20000);
    ASSERT_EQ(ranges[0].second, 20000);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
