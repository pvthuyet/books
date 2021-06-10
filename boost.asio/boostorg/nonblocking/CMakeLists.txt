add_executable(nonblocking third_party_lib.cpp)
target_include_directories(nonblocking PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(nonblocking PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})