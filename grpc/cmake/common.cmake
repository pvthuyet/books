cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

add_compile_definitions(DEBUG_ON) # for write debug spdlog

set(CMAKE_CXX_STANDARD 20)

set(_GRPC_INCLUDEDIR "${GRPC_ROOTDIR}/include")

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
message(STATUS "GRPC_ROOTDIR ${GRPC_ROOTDIR}")
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG HINTS "${GRPC_ROOTDIR}/cmake" NO_CACHE REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)

# find protoc.exe
find_program(_PROTOBUF_PROTOC protoc HINTS "${GRPC_ROOTDIR}/bin" NO_CACHE REQUIRED NO_DEFAULT_PATH)
if (NOT _PROTOBUF_PROTOC)
	message(FATAL_ERROR "_PROTOBUF_PROTOC not found")
endif()
message(STATUS "_PROTOBUF_PROTOC: ${_PROTOBUF_PROTOC}")

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(absl CONFIG HINTS "${GRPC_ROOTDIR}/lib/cmake" REQUIRED)
find_package(gRPC CONFIG HINTS "${GRPC_ROOTDIR}/lib/cmake" REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_REFLECTION gRPC::grpc++_reflection)

# find grpc_cpp_plugin.exe
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin HINTS "${GRPC_ROOTDIR}/bin" NO_CACHE REQUIRED)
if (NOT _GRPC_CPP_PLUGIN_EXECUTABLE)
	message(FATAL_ERROR "_GRPC_CPP_PLUGIN_EXECUTABLE not found")
endif()
message(STATUS "_GRPC_CPP_PLUGIN_EXECUTABLE: ${_GRPC_CPP_PLUGIN_EXECUTABLE}")
