# CMakeList.txt : CMake project for ChatBackend, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ChatBackend "ChatBackend.cpp" "ChatBackend.h" "Debug/Debug.h" "Debug/Debug.cpp"  "Socket/Socket.h" "Socket/Socket.cpp" "Utils/Utils.h" "Utils/Utils.cpp" "Database/Types.h" "Database/Database.h" "Database/Database.cpp")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/cert.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/key.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ChatBackend PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
