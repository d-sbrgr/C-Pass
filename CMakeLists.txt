cmake_minimum_required(VERSION 3.29)
project(C_Pass C)

set(CMAKE_C_STANDARD 11)
if(WIN32)
    # Set the path to the OpenSSL installation on Windows
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")  # Adjust to your OpenSSL installation path
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib")
    set(OPENSSL_LIBS "${OPENSSL_LIB_DIR}/libssl.lib" "${OPENSSL_LIB_DIR}/libcrypto.lib")
elseif(APPLE)
    # Set the path to the OpenSSL installation on macOS
    set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_LIB_DIR "${OPENSSL_ROOT_DIR}/lib")
    set(OPENSSL_LIBS "${OPENSSL_LIB_DIR}/libssl.dylib" "${OPENSSL_LIB_DIR}/libcrypto.dylib")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(C_Pass
    src/main.c
    src/login.h
    src/crypto.c
    src/login.c
    src/util.c
    src/password.c
        src/vault_menu.c
        src/vault_menu.h
)

target_link_libraries(C_Pass ${OPENSSL_LIBS})
