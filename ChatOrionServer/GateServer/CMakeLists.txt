cmake_minimum_required(VERSION 3.5)

set(ITEM_NAME GateServer)

project(${ITEM_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

############################### 执行构建proto文件脚本 ###############################
execute_process(
        COMMAND /bin/bash ${CMAKE_SOURCE_DIR}/../protocol/build_proto.sh
        COMMAND cp -rf ${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_SOURCE_DIR}/../bin
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
##################################################################################


###################################### 查找grpc库 ######################
# 设置 gRPC 和 protobuf 安装的根目录路径
set(GRPC_DIR ${CMAKE_SOURCE_DIR}/../../third/lib/linux/grpc)

# 设置 CMake 查找模块路径
list(APPEND CMAKE_PREFIX_PATH ${GRPC_DIR})

# 查找 gRPC 和 protobuf
find_package(Threads REQUIRED)

# 设置 protobuf 模块兼容
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)

# 查找 gRPC 安装路径
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
####################################################################

file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

include_directories(${CMAKE_SOURCE_DIR}/../../third/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
add_executable(${ITEM_NAME} ${SEC_LSIT}
        verify_grpc_client.h
        Settings.cpp
        Settings.h)


# 指定 Boost 安装的根目录
set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/../../third/lib/linux/boost)

# 查找 Boost 库
find_package(Boost REQUIRED COMPONENTS filesystem system)

set(JSONCPP_LIBRARIES ${CMAKE_SOURCE_DIR}/../../third/lib/linux/jsoncpp)

target_link_libraries (${ITEM_NAME} ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        Threads::Threads ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES}/libjsoncpp.so)
