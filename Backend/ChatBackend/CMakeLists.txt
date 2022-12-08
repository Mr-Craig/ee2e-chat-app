# CMakeList.txt : CMake project for ChatBackend, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ChatBackend "ChatBackend.cpp" "ChatBackend.h" "Debug/Debug.h" "Debug/Debug.cpp"  "Socket/Socket.h" "Socket/Socket.cpp" "Utils/Utils.h" "Utils/Utils.cpp" "Database/Database.h" "Database/Database.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ChatBackend PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
