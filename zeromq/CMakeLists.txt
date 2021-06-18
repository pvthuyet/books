cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(boost-asio LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ZERO_MQ_LIBRARYDIR_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libzmq/lib/Debug")
set(ZERO_MQ_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/libzmq/include")
set(CPP_ZMQ_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/cppzmq/include")
set(ZERO_MQ_LIBRARIES "${ZERO_MQ_LIBRARYDIR_DEBUG}/libzmq-v142-mt-sgd-4_3_5.lib")
message(STATUS "ZERO_MQ_LIBRARIES: ${ZERO_MQ_LIBRARIES}")

set(WINDOWS_LIBRARIES "Ws2_32.lib;IPHLPAPI.lib")
### example
add_subdirectory(1-basic/hello-world)
add_subdirectory(1-basic/weather-station)
add_subdirectory(1-basic/parallel-task)
add_subdirectory(2-socket-and-pattern/multiple-sockets)
add_subdirectory(2-socket-and-pattern/router-dealer)
add_subdirectory(2-socket-and-pattern/handling-interrupt-signals)