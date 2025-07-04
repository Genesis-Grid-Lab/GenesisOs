cmake_minimum_required(VERSION 3.16)
project(GenesisOS)

# build output directories

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_PDB_OUTPUT_DIRECTORY $<0:>${CMAKE_BINARY_DIR}/lib)
# set(EXECUTABLE_OUTPUT_PATH $<0:>${CMAKE_BINARY_DIR}/bin)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# enable console logging

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DUE_DEBUG)
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

add_definitions(-D_POSIX_C_SOURCE=199309L)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/spdlog)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)
set(YAML_CPP_BUILD_CONTRIB OFF)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_BUILD_SHARED_LIBS OFF)
add_subdirectory(vendor/yaml-cpp)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY ON)
add_subdirectory(vendor/JoltPhysics/Build)
add_subdirectory(vendor/ImGuiFileDialog)

add_subdirectory(Engine)
add_subdirectory(App)

install(TARGETS genesisos DESTINATION bin)
install(TARGETS spdlog DESTINATION lib)  # if it builds as a shared lib

