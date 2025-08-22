set(ITEM_NAME orion-message)
project(${ITEM_NAME} LANGUAGES CXX)

# 源文件
#file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp")
#file(GLOB_RECURSE SEC_LSIT "*cpp" "*.cc" "*.cxx")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/message)

add_library(${ITEM_NAME} SHARED # ${SEC_LSIT} ${HEADER_FILES}
        message/message_type.h
        message/message_base.h
        message/message_base.cpp
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

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
