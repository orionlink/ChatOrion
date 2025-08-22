set(ITEM_NAME test-orion-message)
# 确保输出目录存在
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../../bin)
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# 设置可执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# 添加include路径
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../message
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# 源文件
#file(GLOB_RECURSE HEADER_FILES "*.h" "*.hpp")
#file(GLOB_RECURSE SEC_LSIT "*cpp" "*.cc" "*.cxx")

link_directories(${PROJECT_SOURCE_DIR}/../../lib)
add_executable(${ITEM_NAME} test_text_chat_message.cpp)

# 链接所需的库
target_link_libraries(${ITEM_NAME}
        PUBLIC
        orion-message
)
