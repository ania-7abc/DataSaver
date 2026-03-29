cmake_minimum_required(VERSION 3.14)
project(DataSaver VERSION 1.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(DataSaver INTERFACE)
target_include_directories(DataSaver INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/DataSaverConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS DataSaver EXPORT DataSaverTargets)
install(EXPORT DataSaverTargets DESTINATION lib/cmake/DataSaver)
install(DIRECTORY include/ DESTINATION include)

export(TARGETS DataSaver FILE DataSaverTargets.cmake)
