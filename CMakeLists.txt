cmake_minimum_required(VERSION 3.20)

project("ICM_20948_I2C_DMA" C CXX ASM)

include(cmake/st-project.cmake)

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    "src"
    # "src/lib"
    "src/app"
    "src/hw"
    "src/hw/bsp"
    "src/hw/driver"
    "src/hw/driver/inc"
)

target_link_options(
    ${PROJECT_NAME} PRIVATE
    
    -u _printf_float
)