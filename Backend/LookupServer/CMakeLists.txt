cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (LookupServer "LookupServer.cpp" "LookupServer.h")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/cert.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/key.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LookupServer PROPERTY CXX_STANDARD 20)
endif()
