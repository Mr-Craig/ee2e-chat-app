# CMakeList.txt : CMake project for ChatBackend, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ChatBackend "ChatBackend.cpp" "ChatBackend.h" "Debug/Debug.h" "Debug/Debug.cpp"  "Socket/Socket.h" "Socket/Socket.cpp" "Types/Types.h" "Database/Database.h" "Database/Database.cpp" "Handlers/Handlers.h" "Handlers/Relay.h"  "Handlers/WebAuthN.h" "Handlers/Login.h" "Handlers/UsernameCheck.h" "Notifications/Notifications.h" "Notifications/Notifications.cpp" "Handlers/Notifications.h" "Notifications/Vapid.h" "Utils/Http.h" "Utils/Misc.h" "Utils/Argon2.h" "Utils/Crypto.h" "Utils/WebAuthN.h" "Utils/Pinger.h")
add_executable (ChatBackend_Tests "Tests/Argon2.Tests.cpp" "Debug/Debug.h" "Debug/Debug.cpp"  "Socket/Socket.h" "Socket/Socket.cpp" "Types/Types.h" "Database/Database.h" "Database/Database.cpp" "Handlers/Handlers.h" "Handlers/Relay.h"  "Handlers/WebAuthN.h" "Handlers/Login.h" "Handlers/UsernameCheck.h" "Notifications/Notifications.h" "Notifications/Notifications.cpp" "Handlers/Notifications.h" "Notifications/Vapid.h" "Utils/Http.h" "Utils/Misc.h" "Utils/Argon2.h" "Utils/Crypto.h" "Utils/WebAuthN.h" "Utils/Pinger.h" "Tests/Crypto.Tests.cpp" "Tests/Misc.Tests.cpp" "Tests/Vapid.Tests.cpp" "Tests/ECE.Tests.cpp")

target_link_libraries(
  ChatBackend_Tests PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ChatBackend_Tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/cert.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Certs/key.pem
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ChatBackend PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
