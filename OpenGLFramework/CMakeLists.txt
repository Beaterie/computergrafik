cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (OpenGL_Framework)

#set the build type if its not set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
endif()

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

#before adding libraries, define the output paths
# MSVC & Xcode automatically create the build-type folders
if(MSVC OR CMAKE_GENERATOR STREQUAL Xcode)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
  SET(LIBRARY_OUTPUT_PATH	 ${PROJECT_BINARY_DIR})
else()
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
  SET(LIBRARY_OUTPUT_PATH	 ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

# include glm, as system header to suppress warnings
include_directories(SYSTEM external/glm-0.9.6.3)

# include stb_image, as system header to suppress warnings
include_directories(SYSTEM external/stb_image-2.0.6)

# add tinyobjloader
include_directories(external/tinyobjloader-aa07206)
# just one file so it can be directly compiled into main exe
file(GLOB TINYOBJLOADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyobjloader-aa07206/*.cc)

# glfw
# configure glfw building
option(GLFW_BUILD_DOCS     OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS    OFF)
option(GLFW_INSTALL        OFF)
   
# add glfw build system
add_subdirectory(external/glfw-3.1.1)
# include glfw headers
include_directories(external/glfw-3.1.1/include)

# add glbindings
add_subdirectory(external/glbinding-2.1.1)

# create framework helper library 
file(GLOB FRAMEWORK_SOURCES framework/source/*.cpp)
add_library(framework STATIC ${FRAMEWORK_SOURCES} ${TINYOBJLOADER_SOURCES})
target_include_directories(framework PUBLIC framework/include)
target_link_libraries(framework glbinding glfw ${GLFW_LIBRARIES})

# include headers in all following applications
include_directories(application/include)

add_executable(solar_system application/source/application_solar.cpp)
target_link_libraries(solar_system framework)

# MacOS doesnt support simple compat mode required for examples
if(NOT APPLE)
  # add setting whether examples are build
  option(BUILD_EXAMPLES     OFF)

  if(BUILD_EXAMPLES) 
    add_executable(1_fixed application/source/application_fixed.cpp)
    target_link_libraries(1_fixed framework)

    add_executable(2_vbo application/source/application_vbo.cpp)
    target_link_libraries(2_vbo framework)

    add_executable(3_indexed application/source/application_indexed.cpp)
    target_link_libraries(3_indexed framework)

    add_executable(4_shader application/source/application_shader.cpp)
    target_link_libraries(4_shader framework)

    add_executable(5_uniform application/source/application_uniform.cpp)
    target_link_libraries(5_uniform framework)

    add_executable(6_vao application/source/application_vao.cpp)
    target_link_libraries(6_vao framework)
  endif()
endif()

# set build type dependent flags
if(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
elseif(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "/MD /O2")
	set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")
endif()

# activate C++ 11
if(NOT MSVC)
    add_definitions(-std=c++11)
    # show all warnings
    add_definitions(-Wall -Wconversion)
    # force linking with c++11 lib
    if(APPLE)
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        add_definitions(-stdlib=libc++)
    endif()
else()
    # build in parallel, show warnings and suppress one caused by glbinding
    add_definitions(/MP /W3 /wd4251)
endif()

# remove external configuration vars from cmake gui
mark_as_advanced(OPTION_SELF_CONTAINED)
mark_as_advanced(GLFW_BUILD_DOCS GLFW_BUILD_TESTS GLFW_INSTALL GLFW_BUILD_EXAMPLES
 GLFW_DOCUMENT_INTERNALS GLFW_USE_EGL GLFW_USE_MIR GLFW_USE_WAYLAND GLFW_LIBRARIES
 LIB_SUFFIX BUILD_SHARED_LIBS)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# installation rules, copy over binaries to bin
# install(TARGETS OpenGLFramework DESTINATION bin)
