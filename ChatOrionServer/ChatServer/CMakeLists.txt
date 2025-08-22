set(ITEM_NAME ChatServer)
project(${ITEM_NAME} LANGUAGES CXX)

file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

# 添加可执行文件
add_executable(${ITEM_NAME} ${SEC_LSIT}
        CServer.cpp
        CServer.h
        CSession.cpp
        CSession.h
        MsgNode.cpp
        MsgNode.h
        LogicSystem.cpp
        LogicSystem.h
        ChatGrpcClient.cpp
        ChatGrpcClient.h
        UserMgr.cpp
        UserMgr.h
        ChatServiceImpl.cpp
        ChatServiceImpl.h
        data.h
        MySQLManager.h
        MySQLManager.cpp
        MySQLDao.h
        MySQLDao.cpp
        redis_keys.h
        ChatCacheManager.cpp
        ChatCacheManager.h
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