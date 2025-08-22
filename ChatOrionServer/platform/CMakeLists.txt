set(ITEM_NAME platform)
project(${ITEM_NAME} LANGUAGES CXX)

# 源文件
file(GLOB HEADER_FILES "*.h" "*.hpp")
file(GLOB SEC_LSIT "*cpp" "*.cc" "*.cxx")

add_library(${ITEM_NAME} SHARED ${SEC_LSIT} ${HEADER_FILES})

# 链接所需的库
target_link_libraries(${ITEM_NAME}
        PUBLIC
        Threads::Threads
        ${Boost_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        ${HIREDIS_LIBRARIES}
        mysql::concpp-xdevapi
        mysql::concpp-jdbc
        glog::glog
)
