include(../cmake/common.cmake)

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
	${_GRPC_INCLUDEDIR}
)
target_link_libraries(hw_grpc_proto
	${_REFLECTION}
	${_GRPC_GRPCPP}
	${_PROTOBUF_LIBPROTOBUF}
)

foreach(_target
	greeter_client greeter_server)
	add_executable(${_target} "${_target}.cpp")
	target_include_directories(${_target} PRIVATE 
		${_GRPC_INCLUDEDIR}
		${CMAKE_CURRENT_SOURCE_DIR}
		)
	target_link_libraries(${_target} PRIVATE 
		hw_grpc_proto
		${_REFLECTION}
		${_GRPC_GRPCPP}
		${_PROTOBUF_LIBPROTOBUF}
		)
endforeach()