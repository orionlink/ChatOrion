set(ITEM_NAME GateServer)
project(${ITEM_NAME} LANGUAGES CXX)

# 源文件
file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

# 添加可执行文件
add_executable(${ITEM_NAME} ${SEC_LSIT}
        verify_grpc_client.h
        verify_grpc_client.cpp
        MySQLManager.cpp
        MySQLManager.h
        MySQLDao.cpp
        MySQLDao.h
)

# 链接库
target_link_libraries(${ITEM_NAME}
        PRIVATE
        ${PLATFORM_LIBRARIES}
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        Threads::Threads
        ${Boost_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        mysql::concpp-xdevapi
        mysql::concpp-jdbc
        glog::glog
)
