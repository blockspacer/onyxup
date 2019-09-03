cmake_minimum_required(VERSION 3.10)
project(tests)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)

add_executable(common-tests common-tests.cpp)
add_executable(parse-params-request-tests parse-params-request-tests.cpp)
add_executable(parse-ranges-request-tests parse-ranges-request-tests.cpp)
add_executable(url-encoded-tests url-encoded-tests.cpp)
add_executable(multipart-form-data-tests multipart-form-data-tests.cpp)

target_link_libraries(common-tests ${GTEST_LIBRARIES} onyxup pthread curl)
target_link_libraries(parse-params-request-tests ${GTEST_LIBRARIES} onyxup pthread curl)
target_link_libraries(parse-ranges-request-tests ${GTEST_LIBRARIES} onyxup pthread curl)
target_link_libraries(url-encoded-tests ${GTEST_LIBRARIES} onyxup pthread curl)
target_link_libraries(multipart-form-data-tests ${GTEST_LIBRARIES} onyxup pthread curl)

add_test(common-tests "./common-tests")
add_test(parse-params-request-tests "./parse-params-request-tests")
add_test(parse-ranges-request-tests "./parse-ranges-request-tests")
add_test(url-encoded-tests "./url-encoded-tests")
add_test(multipart-form-data-tests "./multipart-form-data-tests")
