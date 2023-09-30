cmake_minimum_required(VERSION 3.21)

project(PydewValley C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Configure project
add_library(CommonIncludes INTERFACE)
target_include_directories(CommonIncludes INTERFACE
    ${SFML_SOURCE_DIR}/include
    ${yaml-cpp_SOURCE_DIR}/include
)

set(PydewValleyIncludes
    src
    src/Game
)

set(PydewValleyCoreIncludes
    src
)

file(GLOB_RECURSE PydewValleyHeaders
    "src/Game/*.h"
)

file(GLOB_RECURSE PydewValleySources
    "src/Game/*.cpp"
)

file(GLOB_RECURSE PydewValleyCoreHeaders
    "src/Core/*.h"
)

file(GLOB_RECURSE PydewValleyCoreSources
    "src/Core/*.cpp"
)

add_library(PydewValleyLibrary STATIC
    ${PydewValleyHeaders}
    ${PydewValleySources}
)

add_library(PydewValleyCoreLibrary STATIC
    ${PydewValleyCoreHeaders}
    ${PydewValleyCoreSources}
)

target_link_libraries(PydewValleyLibrary PUBLIC CommonIncludes)

target_link_libraries(PydewValleyCoreLibrary PUBLIC 
    CommonIncludes
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    yaml-cpp
)

target_link_libraries(PydewValleyLibrary PUBLIC PydewValleyCoreLibrary)

target_include_directories(PydewValleyLibrary PUBLIC
    ${PydewValleyIncludes}
)

target_include_directories(PydewValleyCoreLibrary PUBLIC
    ${PydewValleyCoreIncludes}
)

add_executable(PydewValley 
    bootstrap/Main.cpp
 )

target_link_libraries(PydewValley PUBLIC PydewValleyLibrary PydewValleyCoreLibrary)