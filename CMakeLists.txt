cmake_minimum_required(VERSION 3.15...4.1)

project(ColloLog VERSION 0.2.0 LANGUAGES CXX)

include(GNUInstallDirs)

set (SOURCES
    src/Enums.cpp
    src/ColloLogger.cpp
    src/LocalLogger.cpp
    src/ColloLog.cpp
    src/CollogUtils.cpp
)

set(INSTALL_DIR ${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if (MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        DEBUG_POSTFIX d
    )
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${INSTALL_DIR}>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# [Optimization stuff]

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO (LTO) not supported: ${ipo_error}")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3 -march=native -flto>
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/O2 /GL>
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/LTCG"
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

# [Install stuff]

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION
        $<IF:$<CONFIG:Debug>,debug/lib/${INSTALL_DIR},lib/${INSTALL_DIR}>
)

install(
    DIRECTORY include/
    DESTINATION include/${INSTALL_DIR}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION share/${INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION share/${INSTALL_DIR}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION share/${INSTALL_DIR}
)