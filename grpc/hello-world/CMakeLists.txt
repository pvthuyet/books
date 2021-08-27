add_compile_definitions(DEBUG_ON) # for write debug spdlog

set(GRPC_BINDIR "${SGLIB_ROOT}/${GRPC_NAME}/bin")
set(PROTOBUF_BINDIR "${SGLIB_ROOT}/${PROTOBUF_NAME}/protobuf")

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(GRPC_BINDIR "${GRPC_BINDIR}/x64")
	set(PROTOBUF_BINDIR "${PROTOBUF_BINDIR}/x64")
else()
	set(GRPC_BINDIR "${GRPC_BINDIR}/win32")
	set(PROTOBUF_BINDIR "${PROTOBUF_BINDIR}/win32")
endif()

find_program(_PROTOBUF_PROTOC protoc HINTS "${PROTOBUF_BINDIR}")
if (NOT _PROTOBUF_PROTOC)
	message(FATAL_ERROR "_PROTOBUF_PROTOC not found")
endif()
message(STATUS "_PROTOBUF_PROTOC: ${_PROTOBUF_PROTOC}")

find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin HINTS ${GRPC_BINDIR})
if (NOT _GRPC_CPP_PLUGIN_EXECUTABLE)
	message(FATAL_ERROR "_GRPC_CPP_PLUGIN_EXECUTABLE not found")
endif()
message(STATUS "_GRPC_CPP_PLUGIN_EXECUTABLE: ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

# Proto file
get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/helloworld.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)
message(STATUS "hw_proto_path: ${hw_proto_path}")

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")
add_custom_command(
	OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
	COMMAND ${_PROTOBUF_PROTOC}
	ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
	--cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
	-I"${hw_proto_path}"
	--plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
	"${hw_proto}"
	DEPENDS "${hw_proto}"
)
#include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(hw_grpc_proto STATIC
	${hw_grpc_srcs}
	${hw_proto_srcs}
)
target_include_directories(hw_grpc_proto PRIVATE
	${SGLIB_INCLUDEDIR}
)
target_link_libraries(hw_grpc_proto
	${SGLIB_LIBRARIES}
)

add_executable(greeter-client 
	"greeter_client.cpp"
)
target_include_directories(greeter-client PRIVATE ${SGLIB_INCLUDEDIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(greeter-client PRIVATE 
	hw_grpc_proto
	${SGLIB_LIBRARIES}
)