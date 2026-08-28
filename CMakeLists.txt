cmake_minimum_required(VERSION 3.20)

project(FetchTop VERSION 1.0.0 LANGUAGES CXX)

add_executable(fetchtop 
    src/main.cpp
    src/parsers.cpp
)

target_compile_features(fetchtop PRIVATE cxx_std_20)

target_include_directories(fetchtop PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)