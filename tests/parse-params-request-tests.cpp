#include <gtest/gtest.h>
#include <iostream>
#include <curl/curl.h>

#include "../sources/server/server.h"
#include "../sources/server/utils.h"

class ParseParamsRequestTests : public ::testing::Test {

public:

    ParseParamsRequestTests() {
    }

    ~ParseParamsRequestTests() {
    }

    void SetUp() {

    }

    void TearDown() {
    }

};

TEST_F(ParseParamsRequestTests, Test_1) {
    const char * uri = "/test?id=1";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "1");
}
TEST_F(ParseParamsRequestTests, Test_2) {
    const char * uri = "/test?id=12";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "12");
}
TEST_F(ParseParamsRequestTests, Test_3) {
    const char * uri = "/test?id=123";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "123");
}

TEST_F(ParseParamsRequestTests, Test_4) {
    const char * uri = "/test?param=1";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["param"].c_str(), "1");
}
TEST_F(ParseParamsRequestTests, Test_5) {
    const char * uri = "/test?param=12";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["param"].c_str(), "12");
}
TEST_F(ParseParamsRequestTests, Test_6) {
    const char * uri = "/test?param=123";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["param"].c_str(), "123");
}

TEST_F(ParseParamsRequestTests, Test_7) {
    const char * uri = "/test?id=1&param=1";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "1");
    ASSERT_STREQ(params["param"].c_str(), "1");
}

TEST_F(ParseParamsRequestTests, Test_8) {
    const char * uri = "/test?id=12&param=12";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "12");
    ASSERT_STREQ(params["param"].c_str(), "12");
}

TEST_F(ParseParamsRequestTests, Test_9) {
    const char * uri = "/test?id=123&param=123";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_STREQ(params["id"].c_str(), "123");
    ASSERT_STREQ(params["param"].c_str(), "123");
}

TEST_F(ParseParamsRequestTests, Test_10) {
    const char * uri = "/test";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 0);
}

TEST_F(ParseParamsRequestTests, Test_11) {
    const char * uri = "/test?";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 0);
}

/*
 * У параметра допускается значение пустой строки
 */
TEST_F(ParseParamsRequestTests, Test_12) {
    const char * uri = "/test?id=&param=13";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 2);
    ASSERT_STREQ(params["id"].c_str(), "");
    ASSERT_STREQ(params["param"].c_str(), "13");
}

/*
 * Пары не подходящие под формат param=value отбрасываются, знак = обязателен
 */
TEST_F(ParseParamsRequestTests, Test_13) {
    const char * uri = "/test?id&param=13";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 1);
    ASSERT_STREQ(params["param"].c_str(), "13");
}

TEST_F(ParseParamsRequestTests, Test_14) {
    const char * uri = "/test?param=13&";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 1);
    ASSERT_STREQ(params["param"].c_str(), "13");
}

TEST_F(ParseParamsRequestTests, Test_15) {
    const char * uri = "/test?param=13&id";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 1);
    ASSERT_STREQ(params["param"].c_str(), "13");
}
TEST_F(ParseParamsRequestTests, Test_16) {
    const char * uri = "/test?param=13&id=";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto params = request->getParams();
    ASSERT_EQ(params.size(), 2);
    ASSERT_STREQ(params["id"].c_str(), "");
    ASSERT_STREQ(params["param"].c_str(), "13");
}

TEST_F(ParseParamsRequestTests, Test_17) {
const char * uri = "/test?param=test";
onyxup::PtrRequest request = onyxup::req::requestFactory();
request->setFullURI(uri, strlen(uri));
onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
auto params = request->getParams();
ASSERT_EQ(strlen(params["param"].c_str()), 4);
}

TEST_F(ParseParamsRequestTests, Test_18) {
    const char * uri = "video-stream-source/fetch?camera-guid=2e802662-9686-4b29-9906-d57bd7d62f25";
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    request->setFullURI(uri, strlen(uri));
    onyxup::utils::parseParamsRequest(request, request->getFullURIRef().size());
    auto & params = request->getParams();
    ASSERT_EQ(strlen(params.at("camera-guid").c_str()), 36);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
