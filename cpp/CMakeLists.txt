cmake_minimum_required(VERSION 3.12)

project(rpc)

add_library(${PROJECT_NAME}
	lib/rpc/dispatcher.cc
	lib/rpc/server.cc
	lib/rpc/client.cc
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)