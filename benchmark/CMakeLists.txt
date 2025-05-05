cmake_minimum_required(VERSION 3.15...3.31)
project(collo-log LANGUAGES CXX)

set(SOURCES
    benchmarks.cpp
    main.cpp
    ../src/ThreadPool.cpp
    ../src/RingBuffer.cpp
    ../src/ColloEnums.cpp
    ../src/ColloLogger.cpp
    ../src/LocalLogger.cpp
    ../src/RingLogger.cpp
    ../src/RingLocal.cpp
    ../src/NaiveLogger.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(
    ${PROJECT_NAME} 
    PROPERTIES 
    CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
    CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC 
    .
    ../include
)