cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(boost-asio LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "../../boost_1_74_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# Note boost 1.75.0 error
find_package(Boost 1.74.0 REQUIRED COMPONENTS date_time regex thread)
if(NOT Boost_FOUND)
	message(FATAL_ERROR "Not found Boost version 1.74.0")
endif()

# Result
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_LIB_VERSION: ${Boost_LIB_VERSION}")
message(STATUS "Boost_MAJOR_VERSION: ${Boost_MAJOR_VERSION}")
message(STATUS "Boost_MINOR_VERSION: ${Boost_MINOR_VERSION}")
message(STATUS "Boost_SUBMINOR_VERSION: ${Boost_SUBMINOR_VERSION}")
message(STATUS "Boost_LIB_DIAGNOSTIC_DEFINITIONS: ${Boost_LIB_DIAGNOSTIC_DEFINITIONS}")

### Chapter 01
# 1. create endpoint
add_subdirectory(chapter01/create-endpoint)
# 2. active socket
add_subdirectory(chapter01/active-socket)
# 3. passive socket
add_subdirectory(chapter01/passive-socket)
# 4. resolving DNS
add_subdirectory(chapter01/resolving-DNS)
# 5. Binding socket to endpoint
add_subdirectory(chapter01/socket-endpoint)
# 6. Connecting a socket
add_subdirectory(chapter01/connecting-socket)
# 7. Connecting a socket with dns
add_subdirectory(chapter01/connecting-socket2)
# 8. Accept connection
add_subdirectory(chapter01/accept-connection)

### Chapter 02
add_subdirectory(chapter02)
add_subdirectory(chapter03)

# Network programming
add_subdirectory(network-programming)
add_subdirectory(network-programming/client-server)