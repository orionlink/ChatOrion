cmake_minimum_required(VERSION 3.5)

project(ChatOrionServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB HEADER_FILES "*.h")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

include_directories(${CMAKE_SOURCE_DIR}/../third/include)

add_executable(ChatOrionServer ${SEC_LSIT})


# 指定 Boost 安装的根目录
set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/../third/")

# 查找 Boost 库
find_package(Boost REQUIRED COMPONENTS system filesystem)

set(JSONCPP_LIBRARIES "${CMAKE_SOURCE_DIR}/../third/lib/linux/jsoncpp")

target_link_libraries (ChatOrionServer pthread ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES}/libjsoncpp.so)
