cmake_minimum_required(VERSION 3.21)

project(PydewValley C CXX)

include(FetchContent)

# Add SFML dependency
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

# Configure project
set(PydewValleyIncludes
    src
    ${SFML_SOURCE_DIR}/include    
)

add_library(PydewValleyLibrary STATIC    
    src/Core/Application.h
    src/Core/Application.cpp
    src/Core/IApplicationListener.h
    src/Core/IApplicationListener.cpp    
    src/Core/ILayer.h                
    src/Core/LayerStack.h
    src/Core/LayerStack.cpp
    src/Core/ResourceLocator.h
    src/Core/TextureManager.h
    src/Core/TextureManager.cpp
    src/Game.cpp
    src/Level.h
    src/Settings.h
    src/Settings.cpp
)

target_link_libraries(PydewValleyLibrary PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network    
)

target_include_directories(PydewValleyLibrary PUBLIC
    ${PydewValleyIncludes}
)

add_executable(PydewValley 
    bootstrap/Main.cpp
 )

target_link_libraries(PydewValley PRIVATE PydewValleyLibrary)