cmake_minimum_required(VERSION 3.5)
project(notion)

set(EXTENSION_NAME notion)
set(LOADABLE_EXTENSION_NAME ${EXTENSION_NAME}_extension)

set(EXTENSION_SOURCES
    src/notion_extension.cpp
    src/notion_auth.cpp
    src/notion_requests.cpp
    src/notion_read.cpp
    src/notion_write.cpp
    src/notion_utils.cpp
)

# Find required packages
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(
    src/include
    third_party
)

# Build static extension
build_static_extension(${EXTENSION_NAME} ${EXTENSION_SOURCES})

# Build loadable extension
build_loadable_extension(${EXTENSION_NAME} ${EXTENSION_SOURCES})

# Link OpenSSL
target_link_libraries(${EXTENSION_NAME}_extension OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(${EXTENSION_NAME}_loadable_extension OpenSSL::SSL OpenSSL::Crypto)

# Install
install(
    TARGETS ${EXTENSION_NAME}_extension
    LIBRARY DESTINATION lib
)
