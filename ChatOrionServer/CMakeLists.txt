cmake_minimum_required(VERSION 3.5)

project(ChatOrionServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

# 手动设置 Boost 的库文件路径
set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")
find_package(Boost 1.71 REQUIRED COMPONENTS filesystem system)

message(STATUS "Boost found at: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

#find_package(jsoncpp REQUIRED)
set(JSONCPP_LIBRARIES /usr/local/lib/)

add_executable(ChatOrionServer ${SEC_LSIT})

# 使用 Boost_LIBRARIES 进行链接
target_link_libraries(ChatOrionServer ${Boost_LIBRARIES} ${JSONCPP_LIBRARIES}/libjsoncpp.so)
