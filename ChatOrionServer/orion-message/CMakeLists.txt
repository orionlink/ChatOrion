cmake_minimum_required(VERSION 3.5)

set(ITEM_NAME orion-message)
project(${ITEM_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保输出目录存在
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../lib)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# 设置可执行文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# 执行构建proto文件脚本
execute_process(
        COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/../protocol/build_proto.sh
#        COMMAND cp -rf ${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_SOURCE_DIR}/../bin
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

# 设置third party根目录
set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/../../third)

# 设置 gRPC 的路径列表
set(GRPC_PATHS
        "${THIRD_PARTY_ROOT}/linux/grpc"
)

## 设置 Boost 的路径列表
#set(BOOST_PATHS
#        "${THIRD_PARTY_ROOT}/linux/boost"
#)

# 设置 jsoncpp 的路径列表
set(JSONCPP_PATHS
        "${THIRD_PARTY_ROOT}/linux/jsoncpp"
)

## 设置 hredis 的路径列表
#set(HIREDIS_PATHS
#        "${THIRD_PARTY_ROOT}/linux/hiredis"
#)
#
## 设置 mysql_connector 的路径列表
#set(MYSQL_CONNECTOR_PATHS
#        "${THIRD_PARTY_ROOT}/linux/mysql_connector"
#)

# 设置 glog 的路径列表
set(GLOG_PATHS
        "${THIRD_PARTY_ROOT}/linux/glog"
)

# 将所有路径加入到 CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH ${GRPC_PATHS} ${JSONCPP_PATHS} ${GLOG_PATHS})

# 添加include路径
include_directories(
        ${GRPC_PATHS}/include
        ${JSONCPP_PATHS}/include
        ${GLOG_PATHS}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/message
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

find_package (glog 0.5.0 REQUIRED)

# JsonCpp配置
set(JSONCPP_LIBRARIES ${JSONCPP_PATHS}/lib/libjsoncpp.so)

# 源文件
file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp")
file(GLOB_RECURSE SEC_LSIT "*cpp" "*.cc" "*.cxx")

add_library(${ITEM_NAME} SHARED ${SEC_LSIT} ${HEADER_FILES}
        message/message_type.h
        utils.h
        message/text_chat_message.cpp
        message/text_chat_message.h
        message_factory.cpp
        message_factory.h)

# 链接所需的库
target_link_libraries(${ITEM_NAME}
        PUBLIC
        Threads::Threads
        ${JSONCPP_LIBRARIES}
        glog::glog
)
