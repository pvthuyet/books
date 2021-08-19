add_compile_definitions(DEBUG_ON) # for write debug spdlog

add_executable(Proto2Exam 
	"result.pb.cc"
	"defining_service.pb.cc"
	"main.cpp"
)
target_include_directories(Proto2Exam PRIVATE ${SGLIB_INCLUDEDIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Proto2Exam PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})