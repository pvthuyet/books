add_executable(chapter05 main.cpp "http_errors.hpp" "http_client.hpp" "http_server.hpp" "sync_ssl_client.hpp")
target_include_directories(chapter05 PRIVATE ${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(chapter05 PRIVATE ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})