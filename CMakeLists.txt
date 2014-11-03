cmake_minimum_required(VERSION 2.8)

project(clarus CXX)

include_directories(
    "/usr/local"
    "../Clarus/include"
    "include"
)

find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(OpenCV 2.4.8 REQUIRED)

add_library(cight
    "include/cight/drift_estimator.hpp"
    "include/cight/drift_recorder.hpp"
    "include/cight/drift_reduce.hpp"
    "include/cight/image_stream.hpp"
    "include/cight/replay_stream.hpp"
    "include/cight/sensor_stream.hpp"
    "include/cight/transforms.hpp"
    "include/cight/memory.hpp"
    "src/cight/drift_estimator.cpp"
    "src/cight/drift_recorder.cpp"
    "src/cight/drift_reduce.cpp"
    "src/cight/image_stream.cpp"
    "src/cight/replay_stream.cpp"
    "src/cight/transforms.cpp"
    "src/cight/memory.cpp"
)

install(
    TARGETS
        cight
    DESTINATION "/usr/local/lib"
)

install(
    FILES
        "include/cight/drift_estimator.hpp"
        "include/cight/drift_recorder.hpp"
        "include/cight/drift_reduce.hpp"
        "include/cight/image_stream.hpp"
        "include/cight/replay_stream.hpp"
        "include/cight/sensor_stream.hpp"
        "include/cight/transforms.hpp"
        "include/cight/working_memory.hpp"
    DESTINATION "/usr/local/include/cight"
)
