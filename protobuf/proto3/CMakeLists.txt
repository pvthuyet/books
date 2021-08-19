add_compile_definitions(DEBUG_ON) # for write debug spdlog

add_executable(Proto3Exam 
	"request.pb.cc"
	"main.cpp"
)
target_include_directories(Proto3Exam PRIVATE ${SGLIB_INCLUDEDIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Proto3Exam PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})