cmake_minimum_required(VERSION 3.15...3.31)
project(ColloLog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/NaiveLogger.cpp
    src/ColloEnums.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
