cmake_minimum_required(VERSION 3.1)

project (cpp)

set (CMAKE_CXX_STANDARD 11)

add_subdirectory(glfw)

include_directories(glfw/include)

add_subdirectory(lazy)
add_subdirectory(d3d11_sample01)
add_subdirectory(ExpressionTemplates01)
#add_subdirectory(ProceduralTexture01)
add_subdirectory(Vulkan01)
