cmake_minimum_required(VERSION 3.29)
project(C_Pass C)

set(CMAKE_C_STANDARD 11)

# Set the path to the OpenSSL installation
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")  # Adjust to your OpenSSL installation path

# Manually set the paths to the OpenSSL libraries and include directories
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib")

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(C_Pass
    src/main.c
        src/login.h
        src/crypto.c
        src/login.c
        src/util.c
        src/password.c)

target_link_libraries(C_Pass ${OPENSSL_LIB_DIR}/libssl.lib ${OPENSSL_LIB_DIR}/libcrypto.lib)
