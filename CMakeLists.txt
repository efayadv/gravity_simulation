cmake_minimum_required(VERSION 3.16)
project(gravity_project_2 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(gravity_project_2 main.cpp)

# Try to find GLFW installed via Homebrew
# (Homebrew puts a config at /opt/homebrew/lib/cmake/glfw3 on Apple Silicon,
#  or /usr/local/lib/cmake/glfw3 on Intel)
find_package(glfw3 QUIET)

if(glfw3_FOUND)
  target_link_libraries(gravity_project_2 PRIVATE glfw)
else()
  # Fallback: include/link Homebrew paths directly (Apple Silicon shown)
  target_include_directories(gravity_project_2 PRIVATE /opt/homebrew/include)
  target_link_directories(gravity_project_2 PRIVATE /opt/homebrew/lib)
  target_link_libraries(gravity_project_2 PRIVATE glfw)
endif()

# macOS frameworks for window + OpenGL context
if(APPLE)
  target_link_libraries(gravity_project_2 PRIVATE
    "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework OpenGL")
endif()

include(CTest)
enable_testing()
include(CPack)