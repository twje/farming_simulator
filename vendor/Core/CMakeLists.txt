cmake_minimum_required(VERSION 3.21)

# Set project name and specify C++ standard
set(PROJECT_NAME CoreLibrary)
project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include external dependencies using FetchContent
include(FetchContent)

# Add yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "")

set(YAML_CPP_BUILD_TESTS OFF)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)

# Add SFML dependency
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

# Configure project-wide include directories
set(CoreIncludes
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_SOURCE_DIR}/include
    ${yaml-cpp_SOURCE_DIR}/include
)

# Gather source files for CoreLibrary
file(GLOB_RECURSE CoreSources
    "src/Core/*.cpp"
)

# Gather header files for CoreLibrary
file(GLOB_RECURSE CoreHeaders
    "include/Core/*.h"
)

# Create CoreLibrary static library
set(CoreLibrary CoreLibrary)
add_library(${CoreLibrary} STATIC    
    ${CoreSources}
    ${CoreHeaders} # not required but makes VS code navigation work for CTRL K,O
)

# Link CoreLibrary against SFML, and yaml-cpp
target_link_libraries(${CoreLibrary} PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    yaml-cpp
)

target_include_directories(${CoreLibrary} PUBLIC
    ${CoreIncludes}
)

add_subdirectory(tests)