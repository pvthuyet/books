add_compile_definitions(DEBUG_ON) # for write debug spdlog

find_program(_PROTOBUF_PROTOC protoc)
if (NOT _PROTOBUF_PROTOC)
	message(FATAL_ERROR "_PROTOBUF_PROTOC not found")
endif()
message(STATUS "_PROTOBUF_PROTOC: ${_PROTOBUF_PROTOC}")

find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
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

add_executable(greeter-client 
	"helloworld.pb.cc"
	"greeter_client.cpp"
)
target_include_directories(greeter-client PRIVATE ${SGLIB_INCLUDEDIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(greeter-client PRIVATE ${SGLIB_LIBRARIES})