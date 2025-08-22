cmake_minimum_required(VERSION 3.5)

#set(ITEM_NAME platform)
#project(${ITEM_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保输出目录存在
set(LIB_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lib)
set(EXEC_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${LIB_OUTPUT_DIR})
file(MAKE_DIRECTORY ${EXEC_OUTPUT_DIR})

# 设置可执行文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_OUTPUT_DIR})
# 执行构建proto文件脚本
execute_process(
        COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/protocol/build_proto.sh
        COMMAND cp -rf ${CMAKE_SOURCE_DIR}/platform/config.ini ${CMAKE_SOURCE_DIR}/bin
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/protocol/
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
set(THIRD_PARTY_ROOT ${CMAKE_SOURCE_DIR}/../third)

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

# 设置 glog 的路径列表
set(GLOG_PATHS
        "${THIRD_PARTY_ROOT}/linux/glog"
)

# 将所有路径加入到 CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH ${GRPC_PATHS} ${BOOST_PATHS} ${JSONCPP_PATHS} ${HREDIS_PATHS} ${MYSQL_CONNECTOR_PATHS} ${GLOG_PATHS})

# 添加include路径
include_directories(
        ${GRPC_PATHS}/include
        ${BOOST_PATHS}/include
        ${JSONCPP_PATHS}/include
        ${HIREDIS_PATHS}/include
        ${MYSQL_CONNECTOR_PATHS}/include
        ${GLOG_PATHS}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/platform
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

find_package (glog 0.5.0 REQUIRED)

# JsonCpp配置
set(JSONCPP_LIBRARIES ${JSONCPP_PATHS}/lib/libjsoncpp.so)

set(HIREDIS_LIBRARIES ${HIREDIS_PATHS}/lib/libhiredis.so)

set(PLATFORM_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/libplatform.so)

add_subdirectory(platform)
add_subdirectory(ChatServer)
add_subdirectory(GateServer)
add_subdirectory(StatusServer)
add_subdirectory(orion-message)