cmake_minimum_required(VERSION 3.15)

project(ColloLogBench VERSION 0.2.0 LANGUAGES CXX)

find_package(ColloLog CONFIG REQUIRED)

set(SOURCES
    benchmarks.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ColloLog::ColloLog
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

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
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion
        $<$<CONFIG:Release>:-O3 -march=native -flto>
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/LTCG"
    )
endif()
