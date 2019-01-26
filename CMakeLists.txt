cmake_minimum_required (VERSION 3.8)
project("GameServer")
add_definitions("-std=c++11 -g -Wall -lpthread")
INCLUDE_DIRECTORIES(zinx)
ADD_SUBDIRECTORY(zinx)
aux_source_directory(./customer DIR_SRCS)
aux_source_directory(./customer/Player DIR_PLAYER_SRCS)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

ADD_EXECUTABLE(game ${DIR_SRCS} ${DIR_PLAYER_SRCS})
target_link_libraries(game zinx protobuf pthread)


