cmake_minimum_required (VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project (CSVReader)

include_directories(include)
add_executable(main main.cpp
src/analyser.cpp
)



target_include_directories(main PRIVATE include)
