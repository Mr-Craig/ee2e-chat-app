cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (DummyClient "Main.cpp" "Socket.h" "Socket.cpp" "../ChatBackend/Debug/Debug.h" "../ChatBackend/Debug/Debug.cpp" "Alex.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LookupServer PROPERTY CXX_STANDARD 20)
endif()
