#include <iostream>
#include <boost/filesystem.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

int main() {
    boost::filesystem::path p("/home/hwk/");
    if (boost::filesystem::exists(p)) {
        std::cout << "Path exists!" << std::endl;
    } else {
        std::cout << "Path does not exist." << std::endl;
    }

    Json::Value root;
    root["id"] = 1001;
    root["data"] = "hello world";
    std::string request = root.toStyledString();
    std::cout << "request is " << request << std::endl;

    Json::Value root2;
    Json::Reader reader;
    reader.parse(request, root2);
    std::cout << "msg id is " << root2["id"] << " msg is " << root2["data"] << std::endl;

    // 使用 StreamWriterBuilder 创建 StreamWriter
    Json::StreamWriterBuilder writerBuilder;
    std::string jsonString = Json::writeString(writerBuilder, root);

    // 输出 JSON 数据
    std::cout << "JSON output: " << jsonString << std::endl;


    return 0;
}
