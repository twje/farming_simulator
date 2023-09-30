cmake_minimum_required(VERSION 3.21)

# Set project name and specify C++ standard
project(PydewValley C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include external dependencies using FetchContent
include(FetchContent)

# Add yaml-cpp
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

# Create an interface library for common includes
add_library(CommonIncludes INTERFACE)
target_include_directories(CommonIncludes INTERFACE
    ${SFML_SOURCE_DIR}/include
    ${yaml-cpp_SOURCE_DIR}/include
)

# Configure project-wide include directories
set(PydewValleyIncludes
    src
    src/Game
)

set(PydewValleyCoreIncludes
    vendor/Core/include
)

# Gather source and header files for PydewValleyLibrary
file(GLOB_RECURSE PydewValleyHeaders
    "src/Game/*.h"
)

file(GLOB_RECURSE PydewValleySources
    "src/Game/*.cpp"
)

# Create PydewValleyLibrary static library
add_library(PydewValleyLibrary STATIC
    ${PydewValleyHeaders}
    ${PydewValleySources}
)

# Gather source and header files for PydewValleyCoreLibrary
file(GLOB_RECURSE PydewValleyCoreHeaders
    "vendor/Core/include/Core/*.h"
)

file(GLOB_RECURSE PydewValleyCoreSources
    "vendor/Core/src/Core/*.cpp"
)

# Create PydewValleyCoreLibrary static library
add_library(PydewValleyCoreLibrary STATIC
    ${PydewValleyCoreHeaders}
    ${PydewValleyCoreSources}
)

# Link PydewValleyLibrary against CommonIncludes
target_link_libraries(PydewValleyLibrary PUBLIC CommonIncludes)

# Link PydewValleyCoreLibrary against CommonIncludes, SFML, and yaml-cpp
target_link_libraries(PydewValleyCoreLibrary PUBLIC 
    CommonIncludes
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    yaml-cpp
)

# Link PydewValleyLibrary against PydewValleyCoreLibrary
target_link_libraries(PydewValleyLibrary PUBLIC PydewValleyCoreLibrary)

# Specify include directories for PydewValleyLibrary and PydewValleyCoreLibrary
target_include_directories(PydewValleyLibrary PUBLIC
    ${PydewValleyIncludes}
)

target_include_directories(PydewValleyCoreLibrary PUBLIC
    ${PydewValleyCoreIncludes}
)

# Create the executable PydewValley and link against PydewValleyLibrary and PydewValleyCoreLibrary
add_executable(PydewValley 
    bootstrap/Main.cpp
)

target_link_libraries(PydewValley PUBLIC PydewValleyLibrary PydewValleyCoreLibrary)