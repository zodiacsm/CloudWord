cmake_minimum_required(VERSION 2.8)

project(CloudWord)

set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")

set(CLOUDWORD CloudWord)

set(CLOUDWORD_SRC
	src/font/font.h 
	src/font/font.cpp
	src/lodepng/lodepng_util.cpp
	src/lodepng/lodepng_util.h
	src/lodepng/lodepng.cpp
	src/lodepng/lodepng.h
	src/main.cpp
	)

if(WIN32)
	set(CLOUDWORD_LINK_LIBRARIES
		SOIL.lib
		freetyped.lib
	)
elseif(APPLE)
	set(CLOUDWORD_LINK_LIBRARIES
		libfreetype.6.dylib
		libsoillib.dylib
	)
endif()

if(WIN32)
	link_directories(${PROJECT_SOURCE_DIR}/lib/windows)
elseif(APPLE)
	link_directories(${PROJECT_SOURCE_DIR}/lib/mac)
endif()

include_directories(include)
include_directories(include/freetype)
include_directories(src/lodepng)
include_directories(src/font)

add_executable(${CLOUDWORD} ${CLOUDWORD_SRC})
target_link_libraries(${CLOUDWORD} ${CLOUDWORD_LINK_LIBRARIES})

if(WIN32)
	add_custom_command(TARGET ${CLOUDWORD} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E 		copy_directory ${PROJECT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources)
	add_definitions("/W3 /D_CRT_SECURE_NO_WARNINGS /wd4819 /nologo")
elseif(APPLE)
	add_custom_command(TARGET ${CLOUDWORD} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E 		copy_directory ${PROJECT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/Debug/resources)
	add_custom_command(TARGET ${CLOUDWORD} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E 		copy_directory ${PROJECT_SOURCE_DIR}/lib/mac /usr/local/lib)
endif(WIN32)