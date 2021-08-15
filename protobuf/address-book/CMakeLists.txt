add_compile_definitions(DEBUG_ON) # for write debug spdlog

add_executable(addressbook "address_book.cpp"
	"addressbook.pb.cc"
)
target_include_directories(addressbook PRIVATE ${SGLIB_INCLUDEDIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(addressbook PRIVATE ${WINDOWS_LIBRARIES} ${SGLIB_LIBRARIES})