cmake_minimum_required(VERSION 2.8)

project(clarus CXX)

include_directories(
    "/usr/local/include"
    "../Clarus/include"
    "include"
)

add_library(cight STATIC
    include/cight/drift_estimator.hpp
    include/cight/drift_recorder.hpp
    include/cight/drift_reduce.hpp
    include/cight/image_stream.hpp
    include/cight/replay_stream.hpp
    include/cight/sensor_stream.hpp
    include/cight/transforms.hpp
    include/cight/working_memory.hpp
    src/cight/drift_estimator.cpp
    src/cight/drift_recorder.cpp
    src/cight/drift_reduce.cpp
    src/cight/image_stream.cpp
    src/cight/replay_stream.cpp
    src/cight/transforms.cpp
    src/cight/working_memory.cpp
)
