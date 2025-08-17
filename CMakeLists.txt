cmake_minimum_required(VERSION 3.15)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ProgramDatabase)
endif()

project(HiddenClockInTaskMgr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to look for vcpkg packages if toolchain isn't set
if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")
endif()

# Find Detours installed via vcpkg
find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

# Main exe
add_executable(Main "Main/Source.cpp")
target_link_libraries(Main PRIVATE user32)
set_target_properties(Main PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

# Injector dll
add_library(HideClock SHARED "Injector/HideClock.cpp" "Injector/detours.h")
target_include_directories(HideClock PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(HideClock PRIVATE ${DETOURS_LIBRARY} user32)
set_target_properties(HideClock PROPERTIES OUTPUT_NAME "HideClock")

# Clock exe
add_executable(Clock "Clock/Source.cpp" "Clock/Source.cpp")