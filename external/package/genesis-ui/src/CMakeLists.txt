cmake_minimum_required(VERSION 3.5)
project(genesis-ui)

add_subdirectory(vendor/glm)
find_package(PkgConfig)
pkg_search_module(DRM libdrm)
pkg_search_module(GBM gbm)
pkg_search_module(EGL egl)
pkg_search_module(GLES glesv2)

add_executable(${PROJECT_NAME} src/main.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include  
     ${CMAKE_CURRENT_SOURCE_DIR}/include/Core  
    # ${CMAKE_SOURCE_DIR}/vendor/entt/src/entt
    # ${CMAKE_SOURCE_DIR}/vendor/glad/include    
    ${CMAKE_SOURCE_DIR}/vendor/glm
    
  )

# link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC      
  # glad    
  glm
  # EnTT     
)



if (DRM_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DRM_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${DRM_LIBRARIES})
endif()

if (GBM_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GBM_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GBM_LIBRARIES})
endif()

if (EGL_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${EGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${EGL_LIBRARIES})
endif()

if (GLES_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLES_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLES_LIBRARIES})
endif()

