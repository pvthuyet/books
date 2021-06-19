cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(boost-asio LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##### saigonlib ROOT
set(SGLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../saigonlib")

set(ZERO_MQ_LIBRARYDIR_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/libzmq/lib/Debug")
set(ZERO_MQ_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/libzmq/include")
set(CPP_ZMQ_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/cppzmq/include")
set(ZERO_MQ_LIBRARIES "${ZERO_MQ_LIBRARYDIR_DEBUG}/libzmq-v142-mt-sgd-4_3_5.lib")
message(STATUS "ZERO_MQ_LIBRARIES: ${ZERO_MQ_LIBRARIES}")

set(WINDOWS_LIBRARIES "Ws2_32.lib;IPHLPAPI.lib")

# gsl
set(GSL_INCLUDEDIR "${SGLIB_ROOT}/GSL-3.1.0/include")

# fmt
set(FMT_INCLUDEDIR "${SGLIB_ROOT}/fmt-7.1.3/include")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(FMT_LIBDIR "${SGLIB_ROOT}/fmt-7.1.3/lib/x64")
else()
	set(FMT_LIBDIR "${SGLIB_ROOT}/fmt-7.1.3/lib/win32")
endif()

# SG library
set(SGLIB_INCLUDEDIR "${GSL_INCLUDEDIR};${FMT_INCLUDEDIR};${ZERO_MQ_INCLUDEDIR};${CPP_ZMQ_INCLUDEDIR}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(SGLIB_LIBRARIES "${FMT_LIBDIR}/fmtd.lib")
else()
	set(SGLIB_LIBRARIES "${FMT_LIBDIR}/fmt.lib")
endif()
message(STATUS "SGLIB_INCLUDEDIR: ${SGLIB_INCLUDEDIR}")
message(STATUS "SGLIB_LIBRARIES: ${SGLIB_LIBRARIES}")


### example
add_subdirectory(1-basic/hello-world)
add_subdirectory(1-basic/weather-station)
add_subdirectory(1-basic/parallel-task)
add_subdirectory(2-socket-and-pattern/multiple-sockets)
add_subdirectory(2-socket-and-pattern/router-dealer)
add_subdirectory(2-socket-and-pattern/handling-interrupt-signals)
add_subdirectory(2-socket-and-pattern/multithreading)
add_subdirectory(2-socket-and-pattern/signal-between-thread)
add_subdirectory(2-socket-and-pattern/node-coordination)
add_subdirectory(2-socket-and-pattern/pub-sub-message-envelope)