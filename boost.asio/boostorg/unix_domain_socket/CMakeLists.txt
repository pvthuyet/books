add_executable(connect_pair connect_pair.cpp)
target_include_directories(connect_pair PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(connect_pair PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})