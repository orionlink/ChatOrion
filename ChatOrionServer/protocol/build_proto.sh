#!/bin/bash

# 获取当前目录的路径
current_dir=$(pwd)

protofile=message.proto
protoc_path=$current_dir/../../third/linux/grpc/bin/protoc
protoc_gen_grpc_path=$current_dir/../../third/linux/grpc/bin/grpc_cpp_plugin

$protoc_path -I="." --grpc_out="." --plugin=protoc-gen-grpc="$protoc_gen_grpc_path" "$protofile"

$protoc_path -I="." --cpp_out="." "$protofile"

# 查找所有 .h, .cpp, .cc, .cxx, .hpp 文件
files_to_copy=$(find "$current_dir" -maxdepth 1 \( -name "*.h" -or -name "*.cpp" -or -name "*.cc" -or -name "*.cxx" -or -name "*.hpp" \))

# 遍历每个文件并将其拷贝到GateServer目录
for file in $files_to_copy; do
    cp -f "$file" "$current_dir/../GateServer"
    cp -f "$file" "$current_dir/../StatusServer"
    cp -f "$file" "$current_dir/../ChatServer"
    cp -f "$file" "$current_dir/../platfrom"
    echo "Copied: $file"
done

# 输出完成消息
echo "All files have been copied to the parent directory."
