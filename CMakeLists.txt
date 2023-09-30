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

add_library(PydewValleyLibrary STATIC
    src/Game/Overlay.h
    src/Game/Player.h
    src/Game/Game.cpp
    src/Game/Level.h
    src/Game/Settings.h
    src/Game/Settings.cpp
)

add_library(PydewValleyCoreLibrary STATIC
    src/Core/ISerializable.h
    src/Core/Timer.h    
    src/Core/ResourceManager.h
    src/Core/ResourceManager.cpp
    src/Core/AnimationManager.h
    src/Core/AnimationManager.cpp
    src/Core/TextureAnimationSequence.h
    src/Core/TextureAnimationSequence.cpp
    src/Core/Application.h
    src/Core/Application.cpp
    src/Core/IApplicationListener.h
    src/Core/IApplicationListener.cpp    
    src/Core/ILayer.h                
    src/Core/LayerStack.h
    src/Core/LayerStack.cpp
    src/Core/ResourceLocator.h
    src/Core/GameObject.h
    src/Core/GameObject.cpp
    src/Core/Scene.h
    src/Core/Group.h
    src/Core/Group.cpp
    src/Core/TextureManager.h
    src/Core/TextureManager.cpp
    src/Core/AnimationManager.h
    src/Core/Animation.h
    src/Core/Animation.cpp
    src/Core/Support.h
    src/Core/Support.cpp
    src/Core/ItemPicker.h
    src/Core/RectUtils.h
    src/Core/RectUtils.cpp
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