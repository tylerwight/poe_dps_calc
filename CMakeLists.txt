cmake_minimum_required(VERSION 3.10)

# Project name
project(dps_calc C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES src/main.c)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Link against libraries
target_link_libraries(${PROJECT_NAME} 
    ${SDL2_LIBRARIES} 
    ${OPENGL_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    ${CMAKE_SOURCE_DIR}/include/libcimgui_sdl.so)

# Set rpath to find shared libraries at runtime
set_target_properties(${PROJECT_NAME} PROPERTIES 
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/include"
)

# Custom clean target
add_custom_target(clean-all 
    COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_NAME} ${CMAKE_BINARY_DIR}/*.o
    COMMENT "Cleaning up binaries and object files"
)