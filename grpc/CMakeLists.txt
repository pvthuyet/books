cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project(grpc-tutorial LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set name
set(GSL_NAME "GSL-3.1.0")
set(FMT_NAME "fmt-7.1.3")
SET(VLD_NAME "vld")
SET(PROTOBUF_NAME "protobuf-3.17.3")
SET(GRPC_NAME "grpc-1.39.1")

##### saigonlib ROOT
set(WINDOWS_LIBRARIES "Ws2_32.lib;IPHLPAPI.lib")
set(SGLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../saigonlib")

## Boost library
set(BOOST_ROOT "${SGLIB_ROOT}/boost_1_74_0")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
# Note boost 1.75.0 error
find_package(Boost 1.74.0 REQUIRED COMPONENTS date_time regex thread filesystem context coroutine serialization)
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

## set include
set(GSL_INCLUDEDIR "${SGLIB_ROOT}/${GSL_NAME}/include")
set(FMT_INCLUDEDIR "${SGLIB_ROOT}/${FMT_NAME}/include")
set(VLD_INCLUDEDIR "${SGLIB_ROOT}/${VLD_NAME}/include")
set(PROTOBUF_INCLUDEDIR "${SGLIB_ROOT}/${PROTOBUF_NAME}/protobuf/include")
set(GRPC_INCLUDEDIR "${SGLIB_ROOT}/${GRPC_NAME}/include")
set(ABSEIL_INCLUDEDIR "${SGLIB_ROOT}/${GRPC_NAME}/third_party/abseil-cpp")

## set lib
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(FMT_LIBDIR "${SGLIB_ROOT}/${FMT_NAME}/lib/x64")
	set(VLD_LIBDIR "${SGLIB_ROOT}/${VLD_NAME}/lib/Win64")
	set(PROTOBUF_LIBDIR "${SGLIB_ROOT}/${PROTOBUF_NAME}/protobuf/lib/x64")
	set(GRPC_LIBDIR "${SGLIB_ROOT}/${GRPC_NAME}/lib/x64")
else()
	set(FMT_LIBDIR "${SGLIB_ROOT}/${FMT_NAME}/lib/win32")
	set(VLD_LIBDIR "${SGLIB_ROOT}/${VLD_NAME}/lib/Win32")
	set(PROTOBUF_LIBDIR "${SGLIB_ROOT}/${PROTOBUF_NAME}/protobuf/lib/win32")
	set(GRPC_LIBDIR "${SGLIB_ROOT}/${GRPC_NAME}/lib/win32")
endif()

## SG library
set(SGLIB_INCLUDEDIR "${GSL_INCLUDEDIR};${FMT_INCLUDEDIR};${VLD_INCLUDEDIR};${Boost_INCLUDE_DIRS};${PROTOBUF_INCLUDEDIR};${GRPC_INCLUDEDIR};${ABSEIL_INCLUDEDIR}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(GRPC_LIBDIR "${GRPC_LIBDIR}_debug")
	set(_PROTOBUF_LIBPROTOBUF "${PROTOBUF_LIBDIR}/libprotobufd.lib;${PROTOBUF_LIBDIR}/libprotobuf-lited.lib;${PROTOBUF_LIBDIR}/libprotocd.lib")
	set(_GRPC_GRPCPP "${GRPC_LIBDIR}/absl_bad_optional_access.lib;${GRPC_LIBDIR}/absl_bad_variant_access.lib;${GRPC_LIBDIR}/absl_base.lib;${GRPC_LIBDIR}/absl_city.lib;${GRPC_LIBDIR}/absl_civil_time.lib;${GRPC_LIBDIR}/absl_cord.lib;${GRPC_LIBDIR}/absl_debugging_internal.lib;${GRPC_LIBDIR}/absl_demangle_internal.lib;${GRPC_LIBDIR}/absl_exponential_biased.lib;${GRPC_LIBDIR}/absl_graphcycles_internal.lib;${GRPC_LIBDIR}/absl_hash.lib;${GRPC_LIBDIR}/absl_hashtablez_sampler.lib;${GRPC_LIBDIR}/absl_int128.lib;${GRPC_LIBDIR}/absl_log_severity.lib;${GRPC_LIBDIR}/absl_malloc_internal.lib;${GRPC_LIBDIR}/absl_raw_hash_set.lib;${GRPC_LIBDIR}/absl_raw_logging_internal.lib;${GRPC_LIBDIR}/absl_spinlock_wait.lib;${GRPC_LIBDIR}/absl_stacktrace.lib;${GRPC_LIBDIR}/absl_status.lib;${GRPC_LIBDIR}/absl_statusor.lib;${GRPC_LIBDIR}/absl_strings.lib;${GRPC_LIBDIR}/absl_strings_internal.lib;${GRPC_LIBDIR}/absl_str_format_internal.lib;${GRPC_LIBDIR}/absl_symbolize.lib;${GRPC_LIBDIR}/absl_synchronization.lib;${GRPC_LIBDIR}/absl_throw_delegate.lib;${GRPC_LIBDIR}/absl_time.lib;${GRPC_LIBDIR}/absl_time_zone.lib;${GRPC_LIBDIR}/absl_wyhash.lib;${GRPC_LIBDIR}/address_sorting.lib;${GRPC_LIBDIR}/cares.lib;${GRPC_LIBDIR}/crypto.lib;${GRPC_LIBDIR}/gpr.lib;${GRPC_LIBDIR}/grpc++.lib;${GRPC_LIBDIR}/grpc++_alts.lib;${GRPC_LIBDIR}/grpc++_error_details.lib;${GRPC_LIBDIR}/grpc++_reflection.lib;${GRPC_LIBDIR}/grpc++_unsecure.lib;${GRPC_LIBDIR}/grpc.lib;${GRPC_LIBDIR}/grpcpp_channelz.lib;${GRPC_LIBDIR}/grpc_plugin_support.lib;${GRPC_LIBDIR}/grpc_unsecure.lib;${GRPC_LIBDIR}/re2.lib;${GRPC_LIBDIR}/ssl.lib;${GRPC_LIBDIR}/upb.lib;${GRPC_LIBDIR}/zlibstaticd.lib")
	set(SGLIB_LIBRARIES "${Boost_LIBRARIES};${FMT_LIBDIR}/fmtd.lib;${VLD_LIBDIR}/vld.lib")
	set(SGLIB_LIBRARIES "${SGLIB_LIBRARIES};${_PROTOBUF_LIBPROTOBUF}")
	set(SGLIB_LIBRARIES "${SGLIB_LIBRARIES};${_GRPC_GRPCPP}")
else()
	set(_PROTOBUF_LIBPROTOBUF "${PROTOBUF_LIBDIR}/libprotobuf.lib;${PROTOBUF_LIBDIR}/libprotobuf-lite.lib;${PROTOBUF_LIBDIR}/libprotoc.lib")
	set(_GRPC_GRPCPP "${GRPC_LIBDIR}/absl_bad_optional_access.lib;${GRPC_LIBDIR}/absl_bad_variant_access.lib;${GRPC_LIBDIR}/absl_base.lib;${GRPC_LIBDIR}/absl_city.lib;${GRPC_LIBDIR}/absl_civil_time.lib;${GRPC_LIBDIR}/absl_cord.lib;${GRPC_LIBDIR}/absl_debugging_internal.lib;${GRPC_LIBDIR}/absl_demangle_internal.lib;${GRPC_LIBDIR}/absl_exponential_biased.lib;${GRPC_LIBDIR}/absl_graphcycles_internal.lib;${GRPC_LIBDIR}/absl_hash.lib;${GRPC_LIBDIR}/absl_hashtablez_sampler.lib;${GRPC_LIBDIR}/absl_int128.lib;${GRPC_LIBDIR}/absl_log_severity.lib;${GRPC_LIBDIR}/absl_malloc_internal.lib;${GRPC_LIBDIR}/absl_raw_hash_set.lib;${GRPC_LIBDIR}/absl_raw_logging_internal.lib;${GRPC_LIBDIR}/absl_spinlock_wait.lib;${GRPC_LIBDIR}/absl_stacktrace.lib;${GRPC_LIBDIR}/absl_status.lib;${GRPC_LIBDIR}/absl_statusor.lib;${GRPC_LIBDIR}/absl_strings.lib;${GRPC_LIBDIR}/absl_strings_internal.lib;${GRPC_LIBDIR}/absl_str_format_internal.lib;${GRPC_LIBDIR}/absl_symbolize.lib;${GRPC_LIBDIR}/absl_synchronization.lib;${GRPC_LIBDIR}/absl_throw_delegate.lib;${GRPC_LIBDIR}/absl_time.lib;${GRPC_LIBDIR}/absl_time_zone.lib;${GRPC_LIBDIR}/absl_wyhash.lib;${GRPC_LIBDIR}/address_sorting.lib;${GRPC_LIBDIR}/cares.lib;${GRPC_LIBDIR}/crypto.lib;${GRPC_LIBDIR}/gpr.lib;${GRPC_LIBDIR}/grpc++.lib;${GRPC_LIBDIR}/grpc++_alts.lib;${GRPC_LIBDIR}/grpc++_error_details.lib;${GRPC_LIBDIR}/grpc++_reflection.lib;${GRPC_LIBDIR}/grpc++_unsecure.lib;${GRPC_LIBDIR}/grpc.lib;${GRPC_LIBDIR}/grpcpp_channelz.lib;${GRPC_LIBDIR}/grpc_plugin_support.lib;${GRPC_LIBDIR}/grpc_unsecure.lib;${GRPC_LIBDIR}/re2.lib;${GRPC_LIBDIR}/ssl.lib;${GRPC_LIBDIR}/upb.lib;${GRPC_LIBDIR}/zlibstatic.lib")
	set(SGLIB_LIBRARIES "${Boost_LIBRARIES};${FMT_LIBDIR}/fmt.lib;${VLD_LIBDIR}/vld.lib")
	set(SGLIB_LIBRARIES "${SGLIB_LIBRARIES};${_PROTOBUF_LIBPROTOBUF}")
	set(SGLIB_LIBRARIES "${SGLIB_LIBRARIES};${_GRPC_GRPCPP}")
endif()
message(STATUS "SGLIB_INCLUDEDIR: ${SGLIB_INCLUDEDIR}")
message(STATUS "SGLIB_LIBRARIES: ${SGLIB_LIBRARIES}")

foreach(flag_var
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
		message(STATUS "before: ${${flag_var}}")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		message(STATUS "after: ${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
endforeach()

### example
add_subdirectory("hello-world")