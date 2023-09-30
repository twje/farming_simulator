cmake_minimum_required(VERSION 3.21)

# Set project name and specify C++ standard
set(PROJECT_NAME PydewValley)
project(${PROJECT_NAME} C CXX)

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

# Configure project-wide include directories
set(GameIncludes
    src
    src/Game
)

set(CoreIncludes
    vendor/Core/include
    ${SFML_SOURCE_DIR}/include
    ${yaml-cpp_SOURCE_DIR}/include
)

# Gather source files for GameLibrary
file(GLOB_RECURSE GameSources
    "src/Game/*.cpp"
)

# Create GameLibrary static library
set(GameLibrary PydewValleyLibrary)
add_library(${GameLibrary} STATIC    
    ${GameSources}
)

# Gather source files for CoreLibrary
file(GLOB_RECURSE CoreSources
    "vendor/Core/src/Core/*.cpp"
)

# Create CoreLibrary static library
set(CoreLibrary PydewValleyCoreLibrary)
add_library(${CoreLibrary} STATIC    
    ${CoreSources}
)

# Link CoreLibrary against CommonIncludes, SFML, and yaml-cpp
target_link_libraries(${CoreLibrary} PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    yaml-cpp
)

# Link GameLibrary against CoreLibrary
target_link_libraries(${GameLibrary} PUBLIC ${CoreLibrary})

# Specify include directories for GameLibrary and CoreLibrary
target_include_directories(${GameLibrary} PUBLIC
    ${GameIncludes}
)

target_include_directories(${CoreLibrary} PUBLIC
    ${CoreIncludes}
)

# Create the executable PydewValley and link against GameLibrary and CoreLibrary
add_executable(${PROJECT_NAME} 
    bootstrap/Main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${GameLibrary})
