cmake_minimum_required(VERSION 3.16)
project(genesisos)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include dirs for your headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# export engine symbols
target_compile_definitions(${PROJECT_NAME} PUBLIC
  -DUE_IMPORT -DUE_ENABLE_ASSERTS
)

# includes directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include        
    ${CMAKE_SOURCE_DIR}/Engine/include
)

# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC             
    UrbanEngine
)

