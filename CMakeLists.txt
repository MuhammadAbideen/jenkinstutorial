cmake_minimum_required(VERSION 3.14)
project(MathDemo VERSION 1.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# Create the math library
add_library(mathlib
    src/mathlib/mathlib.cpp
)

target_include_directories(mathlib
    PUBLIC src
)

# Create the main executable
add_executable(mathdemo
    src/main.cpp
)

# Link libraries to the executable
target_link_libraries(mathdemo
    PRIVATE
        mathlib
        spdlog::spdlog
)
