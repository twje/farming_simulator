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

# Gather source files for CoreLibraryTests
file(GLOB_RECURSE CoreLibraryTestsSources 
    "src/*.cpp"
)

# Setup tests
add_executable(CoreLibraryTests 
    ${CoreLibraryTestsSources}
)

target_link_libraries(CoreLibraryTests PUBLIC 
    gtest_main
    ${CoreLibrary}
)

add_test(
    NAME CoreLibraryTests
    COMMAND CoreLibraryTests
)