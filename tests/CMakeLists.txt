cmake_minimum_required(VERSION 3.21)

# Include external dependencies using FetchContent
include(FetchContent)

# Add googletest dependency
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

# Gather source files for GameLibraryTests
file(GLOB_RECURSE GameLibraryTestsSources 
    "src/*.cpp"
)

# Setup tests
add_executable(GameLibraryTests 
    ${GameLibraryTestsSources}
)

target_link_libraries(GameLibraryTests PUBLIC 
    gtest_main
    ${GameLibrary}
)

add_test(
    NAME GameLibraryTests
    COMMAND GameLibraryTests
)