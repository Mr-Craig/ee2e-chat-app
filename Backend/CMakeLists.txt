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
find_package(unofficial-libargon2 CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(LibCbor CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)

find_path(CPPCODEC_INCLUDE_DIRS "cppcodec/base32_crockford.hpp")
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")

add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Include sub-projects.
add_subdirectory ("ChatBackend")

# no boost
add_compile_definitions(_WEBSOCKETPP_CPP11_RANDOM_DEVICE_)

add_subdirectory ("LookupServer")
add_subdirectory ("DummyClient")
add_subdirectory ("ThirdParty/ecec")

add_compile_definitions(OPENSSL_SUPPRESS_DEPRECATED)

target_link_libraries(DummyClient PRIVATE 
	websocketpp::websocketpp
	${Boost_LIBRARY_DIRS}
	ZLIB::ZLIB
	OpenSSL::SSL
	nlohmann_json::nlohmann_json
	fmt::fmt
	OpenSSL::Crypto)

target_include_directories(DummyClient PRIVATE
	${Boost_INCLUDE_DIRS}
	${CPPCODEC_INCLUDE_DIRS})

target_include_directories(ChatBackend PRIVATE
	${PROJECT_SOURCE_DIR}/ThirdParty/ecec/include
	${UWEBSOCKETS_INCLUDE_DIRS} 
	${LIBCBOR_INCLUDE_DIR} 
	${CPPCODEC_INCLUDE_DIRS})

target_link_libraries(ChatBackend PRIVATE
	${LIBCBOR_LIBRARY}
	ece
	CURL::libcurl
	Threads::Threads
	unofficial::argon2::libargon2
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


target_include_directories(ChatBackend_Tests PRIVATE
	${PROJECT_SOURCE_DIR}/ThirdParty/ecec/include
	${UWEBSOCKETS_INCLUDE_DIRS} 
	${LIBCBOR_INCLUDE_DIR} 
	${CPPCODEC_INCLUDE_DIRS})

target_link_libraries(ChatBackend_Tests PRIVATE
	${LIBCBOR_LIBRARY}
	ece
	CURL::libcurl
	Threads::Threads
	unofficial::argon2::libargon2
	ZLIB::ZLIB
	${USOCKETS_LIB}
	$<IF:$<TARGET_EXISTS:uv_a>,uv_a,uv>
	nlohmann_json::nlohmann_json
	fmt::fmt
	SQLiteCpp
	OpenSSL::SSL
	OpenSSL::Crypto
)