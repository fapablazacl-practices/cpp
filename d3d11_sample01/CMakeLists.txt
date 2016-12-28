
set (target D3D11Sample01)
set (sources D3D11Sample01.cpp)

add_executable(${target} ${sources})

target_link_libraries(${target} d3d11)
