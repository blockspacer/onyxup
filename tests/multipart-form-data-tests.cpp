#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "../sources/server/server.h"
#include "../sources/server/utils.h"
#include "../sources/multipart/MultipartFormDataObject.h"

class MultipartFormDataTests : public ::testing::Test {

public:

    MultipartFormDataTests() {
    }

    ~MultipartFormDataTests() {
    }

    void SetUp() {

    }

    void TearDown() {
    }

};

TEST_F(MultipartFormDataTests, Test_1) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\"field\"\r\n\r\n" << 123 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=--Asrf456BGe4h");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());

    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);
    std::vector<char> data = form_fields["field"].getData();
    std::string value(data.begin(), data.end());
    ASSERT_STREQ(value.c_str(), "123");
}

TEST_F(MultipartFormDataTests, Test_2) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\"field-first\"\r\n\r\n" << 123 << "\r\n--Asrf456BGe4h" <<
            "\r\n" << "Content-Disposition: form-data; name=\"field-second\"\r\n\r\n" << 88123 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=--Asrf456BGe4h");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());

    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);

    std::vector<char> data = form_fields["field-first"].getData();
    std::string value(data.begin(), data.end());
    ASSERT_STREQ(value.c_str(), "123");

    data = form_fields["field-second"].getData();
    value = std::string(data.begin(), data.end());
    ASSERT_STREQ(value.c_str(), "88123");
}

TEST_F(MultipartFormDataTests, Test_3) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\"field\"\r\n\r\n" << 1 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=--Asrf456BGe4h");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());

    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);
    std::vector<char> data = form_fields["field"].getData();
    std::string value(data.begin(), data.end());
    ASSERT_STREQ(value.c_str(), "1");
}

TEST_F(MultipartFormDataTests, Test_4) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);
    ASSERT_EQ(form_fields.empty(), true);
}

TEST_F(MultipartFormDataTests, Test_5) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\r\n\r\n" << 1 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=--Asrf456BGe4h");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());
    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);

    ASSERT_EQ(form_fields.empty(), true);
}

TEST_F(MultipartFormDataTests, Test_6) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\r\n\r\n" << 1 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=--00000000");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());
    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);

    ASSERT_EQ(form_fields.empty(), true);
}

TEST_F(MultipartFormDataTests, Test_7) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\r\n\r\n" << 1 << "\r\n--Asrf456BGe4h0000000";
    request->addHeader("content-type", "multipart/form-data; boundary=--Asrf456BGe4h");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());
    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);

    ASSERT_EQ(form_fields.empty(), true);
}

TEST_F(MultipartFormDataTests, Test_8) {
    onyxup::PtrRequest request = onyxup::req::requestFactory();
    std::ostringstream body;
    body << "\r\n\r\n--Asrf456BGe4h\r\n" << "Content-Disposition: form-data; name=\r\n\r\n" << 1 << "\r\n--Asrf456BGe4h";
    request->addHeader("content-type", "multipart/form-data; boundary=");
    request->addHeader("content-length", std::to_string(body.str().size()));
    request->setBody(body.str().c_str(), body.str().size());
    std::unordered_map<std::string, onyxup::MultipartFormDataObject> form_fields = onyxup::utils::multipartFormData(request);

    ASSERT_EQ(form_fields.empty(), true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
