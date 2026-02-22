cmake_minimum_required(VERSION 3.15)
project(openssl_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

add_executable(openssl_test
    btc-address-gen/btc-address-gen.cpp
)

target_link_libraries(openssl_test
    OpenSSL::SSL
    OpenSSL::Crypto
)