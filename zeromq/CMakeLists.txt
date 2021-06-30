cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(boost-asio LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set name
set(GSL_NAME "GSL-3.1.0")
set(FMT_NAME "fmt-7.1.3")
set(LIBZMQ_NAME "libzmq-4.3.4")
SET(CPPZMQ_NAME "cppzmq-4.7.1")

##### saigonlib ROOT
set(WINDOWS_LIBRARIES "Ws2_32.lib;IPHLPAPI.lib")
set(SGLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../saigonlib")

set(LIBZMQ_INCLUDEDIR "${SGLIB_ROOT}/${LIBZMQ_NAME}/include")
set(CPPZMQ_INCLUDEDIR "${SGLIB_ROOT}/${CPPZMQ_NAME}/include")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(LIBZMQ_DIR "${SGLIB_ROOT}/${LIBZMQ_NAME}/lib/x64")
else()
	set(LIBZMQ_DIR "${SGLIB_ROOT}/${LIBZMQ_NAME}/lib/win32")
endif()

# gsl
set(GSL_INCLUDEDIR "${SGLIB_ROOT}/${GSL_NAME}/include")

# fmt
set(FMT_INCLUDEDIR "${SGLIB_ROOT}/${FMT_NAME}/include")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(FMT_LIBDIR "${SGLIB_ROOT}/${FMT_NAME}/lib/x64")
else()
	set(FMT_LIBDIR "${SGLIB_ROOT}/${FMT_NAME}/lib/win32")
endif()

# SG library
set(SGLIB_INCLUDEDIR "${GSL_INCLUDEDIR};${FMT_INCLUDEDIR};${LIBZMQ_INCLUDEDIR};${CPPZMQ_INCLUDEDIR}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(SGLIB_LIBRARIES "${FMT_LIBDIR}/fmtd.lib;${LIBZMQ_DIR}/libzmq-sd.lib")
else()
	set(SGLIB_LIBRARIES "${FMT_LIBDIR}/fmt.lib;${LIBZMQ_DIR}/libzmq-s.lib")
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
add_subdirectory(3-advanced-request-reply-patterns/identity)
add_subdirectory(3-advanced-request-reply-patterns/load-balancing-pattern)
add_subdirectory(3-advanced-request-reply-patterns/asynchronous)
add_subdirectory(3-advanced-request-reply-patterns/inter-broker-routing)