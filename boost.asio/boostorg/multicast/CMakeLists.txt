add_executable(receiver receiver.cpp)
target_include_directories(receiver PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(receiver PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

add_executable(sender "sender.cpp")
target_include_directories(sender PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(sender PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})