
set (sources Vulkan01.cpp)
set (target Vulkan01)

find_package(Vulkan REQUIRED)

include_directories(${VULKAN_INCLUDE_DIR})

add_executable(${target} ${sources})

target_link_libraries( ${target} ${VULKAN_LIBRARY} glfw)

#glfw ${GLFW_LIBRARIES} 
