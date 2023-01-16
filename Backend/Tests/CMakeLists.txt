cmake_minimum_required (VERSION 3.8)

add_executable(hash "Hashing/hash.cpp" "../ChatBackend/Utils/Utils.h" "../ChatBackend/Utils/Utils.cpp" "../ChatBackend/Debug/Debug.h" "../ChatBackend/Debug/Debug.cpp")
add_executable(verifyHash "Hashing/verifyHash.cpp" "../ChatBackend/Utils/Utils.h" "../ChatBackend/Utils/Utils.cpp" "../ChatBackend/Debug/Debug.h" "../ChatBackend/Debug/Debug.cpp")
add_executable(verifyHashFail "Hashing/verifyHashFail.cpp" "../ChatBackend/Utils/Utils.h" "../ChatBackend/Utils/Utils.cpp" "../ChatBackend/Debug/Debug.h" "../ChatBackend/Debug/Debug.cpp")

find_package(CURL CONFIG REQUIRED)
find_package(unofficial-libargon2 CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(hash PRIVATE
    CURL::libcurl
	Threads::Threads
	unofficial::argon2::libargon2
    fmt::fmt
)

target_link_libraries(verifyHash PRIVATE
    CURL::libcurl
    Threads::Threads
    unofficial::argon2::libargon2
    fmt::fmt
)

target_link_libraries(verifyHashFail PRIVATE
    CURL::libcurl
    Threads::Threads
    unofficial::argon2::libargon2
    fmt::fmt
)


cmake_policy(SET CMP0110 NEW)

add_test("Hash String" hash)
add_test("Verify Hashed String - Matches" verifyHash)
add_test("Verify Hashed String - Shouldn't match" verifyHashFail)