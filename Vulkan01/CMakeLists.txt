
set (sources Vulkan01.cpp)
set (target Vulkan01)

add_executable(${target} ${sources})

target_link_libraries(${target} glfw vulkan)
