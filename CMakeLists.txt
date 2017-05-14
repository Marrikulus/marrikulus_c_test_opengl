cmake_minimum_required(VERSION 3.5.1)
project(game)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

find_package(PkgConfig REQUIRED)

find_package(X11 REQUIRED) # Make sure x is there 
if(NOT X11_FOUND)
    message("ERROR: x11 not found")
endif(NOT X11_FOUND)

find_package(OpenGL REQUIRED) # Make sure OpenGL is available for direct calls
if(NOT OPENGL_FOUND)
    message("ERROR: OpenGL not found")
endif(NOT OPENGL_FOUND)

find_package(glfw3 3.2 REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)


set(GLAD_LIBRARIES dl)


file(GLOB SOURCES "src/*.c")

include_directories(Headers/
					Vendor/glad/
					Vendor/KHR
					Vendor/stb/
					Vendor/math/
					${GLFW_INCLUDE_DIRS}
					${X11_INCLUDE_DIR})

file(GLOB VENDORS_SOURCES Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS Headers/*.h)
file(GLOB PROJECT_SOURCES Sources/*.c)
file(GLOB PROJECT_SHADERS Shaders/*.comp
                          Shaders/*.frag
                          Shaders/*.geom
                          Shaders/*.vert)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(
				-Wall
				-std=c11
				-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})

set(ALL_LIBS
	${OPENGL_gl_LIBRARY}
	${GL_LIBRARY}
	${GLFW_LIBRARIES}
	${X11_LIBRARIES}
	${GLAD_LIBRARIES}
	m Xinerama Xrandr Xi Xcursor pthread
	/usr/lib/x86_64-linux-gnu/libXxf86vm.so
	/usr/lib/x86_64-linux-gnu/libX11.so
)

target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)
