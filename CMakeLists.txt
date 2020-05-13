#
# cmake go/c++ integration thanks to
# https://insujang.github.io/2019-12-10/cmake-for-custom-library-build-system-in-go/
#

cmake_minimum_required(VERSION 3.0)
project(TestProject)
cmake_policy(SET CMP0079 NEW)

# 3rd party tools
find_package(Qt5 COMPONENTS Widgets Qml Quick REQUIRED)

# GUI source code
add_subdirectory(src)

# Go library
add_subdirectory(go)

# MinimalQml is the "inner" Qt project name
target_link_libraries("MinimalQml" goshim pthread)
