# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ChatBackend")

set(CMAKE_CXX_STANDARD 23)

cmake_policy(SET CMP0079 NEW)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_library(USOCKETS_LIB uSockets)
#find_package(unofficial-libuv CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)


find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
find_package(nlohmann_json CONFIG REQUIRED)

find_package(SQLiteCpp CONFIG REQUIRED)

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

# Include sub-projects.
add_subdirectory ("ChatBackend")
add_subdirectory ("LookupServer")

target_include_directories(ChatBackend PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
target_link_libraries(ChatBackend PRIVATE
	CURL::libcurl
	ZLIB::ZLIB
	${USOCKETS_LIB}
	$<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>
	nlohmann_json::nlohmann_json
	fmt::fmt
	SQLiteCpp
	OpenSSL::SSL
	OpenSSL::Crypto
)

target_include_directories(LookupServer PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS})
target_link_libraries(LookupServer PRIVATE
	ZLIB::ZLIB
	${USOCKETS_LIB}
	$<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>
	nlohmann_json::nlohmann_json
	fmt::fmt
	SQLiteCpp
	OpenSSL::SSL
	OpenSSL::Crypto
)
