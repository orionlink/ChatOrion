cmake_minimum_required(VERSION 3.5)

set(ITEM_NAME ChatServer)
project(${ITEM_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保输出目录存在
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../bin)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# 设置可执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# 执行构建proto文件脚本
execute_process(
        COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/../protocol/build_proto.sh
        COMMAND cp -rf ${CMAKE_SOURCE_DIR}/../platfrom/config.ini ${CMAKE_SOURCE_DIR}/../bin
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../protocol/
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_output
)
if(result EQUAL 0)
    message(STATUS "Script executed successfully!")
    message(STATUS "Output: ${output}")
else()
    message(WARNING "Script execution failed with error code ${result}")
    message(WARNING "Error Output: ${error_output}")
endif()

# 设置platfrom
set(PLATFROM_PATHS
        "${CMAKE_SOURCE_DIR}/../platfrom"
)

# 设置third party根目录
set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/../../third)

# 设置 gRPC 的路径列表
set(GRPC_PATHS
        "${THIRD_PARTY_ROOT}/linux/grpc"
)

# 设置 Boost 的路径列表
set(BOOST_PATHS
        "${THIRD_PARTY_ROOT}/linux/boost"
)

# 设置 jsoncpp 的路径列表
set(JSONCPP_PATHS
        "${THIRD_PARTY_ROOT}/linux/jsoncpp"
)

# 设置 hredis 的路径列表
set(HIREDIS_PATHS
        "${THIRD_PARTY_ROOT}/linux/hiredis"
)

# 设置 mysql_connector 的路径列表
set(MYSQL_CONNECTOR_PATHS
        "${THIRD_PARTY_ROOT}/linux/mysql_connector"
)

# 将所有路径加入到 CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH ${PLATFROM_PATHS} ${GRPC_PATHS} ${BOOST_PATHS} ${JSONCPP_PATHS} ${HREDIS_PATHS} ${MYSQL_CONNECTOR_PATHS})

# 添加include路径
include_directories(
        ${PLATFROM_PATHS}
        ${GRPC_PATHS}/include
        ${BOOST_PATHS}/include
        ${JSONCPP_PATHS}/include
        ${HIREDIS_PATHS}/include
        ${MYSQL_CONNECTOR_PATHS}/include
)

# 查找线程库
find_package(Threads REQUIRED)

###############################################################################
# 查找 grpc 依赖
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)

set(_REFLECTION gRPC::grpc++_reflection)
# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
##################################################################################

# Boost配置
#set(BOOST_ROOT ${THIRD_PARTY_ROOT}/lib/linux/boost)
find_package(Boost REQUIRED COMPONENTS filesystem system)

# mysql_connector
find_package(mysql-concpp REQUIRED)

# JsonCpp配置
set(JSONCPP_LIBRARIES ${JSONCPP_PATHS}/lib/libjsoncpp.so)

set(HIREDIS_LIBRARIES ${HIREDIS_PATHS}/lib/libhiredis.so)

# 源文件
file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

set(PLATFROM_LIBRARIES ${CMAKE_SOURCE_DIR}/../lib/libplatfrom.so)

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
)

# 链接库
target_link_libraries(${ITEM_NAME}
        PRIVATE
        ${PLATFROM_LIBRARIES}
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        Threads::Threads
        ${Boost_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        mysql::concpp-xdevapi
        mysql::concpp-jdbc
)

# 打印调试信息
message(STATUS "GRPC_PATHS: ${GRPC_PATHS}")
message(STATUS "BOOST_PATHS: ${BOOST_PATHS}")
message(STATUS "JSONCPP_PATHS: ${JSONCPP_PATHS}")

message(STATUS "JSONCPP_LIBRARIES: ${JSONCPP_LIBRARIES}")