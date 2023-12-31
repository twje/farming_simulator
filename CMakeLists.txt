cmake_minimum_required(VERSION 3.22)

# Set project name and specify C++ standard
set(PROJECT_NAME PydewValley)
project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Configure project-wide include directories
set(GameIncludes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Game
)

# Gather source files for GameLibrary
file(GLOB_RECURSE GameSources 
    "src/*.cpp"
)

# Gather header files for GameLibrary
file(GLOB_RECURSE GameHeaders 
    "src/*.h"
)

# Create GameLibrary static library
set(GameLibrary PydewValleyLibrary)
add_library(${GameLibrary} STATIC 
    ${GameSources}
    ${GameHeaders} # not required but makes VS code navigation work for CTRL K,O
)

add_subdirectory(vendor/Core)

# Link GameLibrary against CoreLibrary
target_link_libraries(${GameLibrary} PUBLIC 
    CoreLibrary
)

# Specify include directories for GameLibrary
target_include_directories(${GameLibrary} PUBLIC 
    ${GameIncludes}
)

# Create the executable PydewValley and link against GameLibrary
add_executable(${PROJECT_NAME} 
    bootstrap/Main.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    ${GameLibrary}
)

add_subdirectory(tests)