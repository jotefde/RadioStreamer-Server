# CMakeList.txt : CMake project for RadioStreamer-Server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("RadioStreamer-Server")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
# Add source to this project's executable.
add_executable (RadioStreamer-Server "main.cpp" "main.h" "Server.cpp" "Server.h" "Enums.h" "Utils.h" "Utils.cpp" "Client.cpp" "Client.h" "Playlist.cpp" "Track.cpp" "Track.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RadioStreamer-Server PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
