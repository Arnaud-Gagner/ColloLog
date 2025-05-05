cmake_minimum_required(VERSION 3.15...3.31)
project(collo-log VERSION 0.2.0 LANGUAGES CXX)

set(
    INSTALL_DIR
    ${PROJECT_NAME}-${PROJECT_VERSION}
)

set(SOURCES
    src/ThreadPool.cpp
    src/RingBuffer.cpp
    src/ColloEnums.cpp
    src/ColloLogger.cpp
    src/LocalLogger.cpp
    src/RingLogger.cpp
    src/RingLocal.cpp
    src/NaiveLogger.cpp
)

add_library(${PROJECT_NAME} ${SOURCES})

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    VERSION ${PROJECT_VERSION}
    CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
    CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
    $<INSTALL_INTERFACE:include/${INSTALL_DIR}>
)

# install
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-config
    ARCHIVE DESTINATION lib/${INSTALL_DIR}
)

install(
    DIRECTORY include/
    DESTINATION include/${INSTALL_DIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
)

export(
    TARGETS ${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
)

install(
    FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION share/${INSTALL_DIR}
)

install(
    EXPORT ${PROJECT_NAME}-config
    DESTINATION share/${INSTALL_DIR}
    NAMESPACE ${PROJECT_NAME}::
)

