//
// Created by hwk on 25-8-22.
//

#include "message_base.h"
#include "message_factory.h"
#include "text_chat_message.h"

#include <memory>
#include <iostream>

using namespace message;
using namespace std;

int main()
{
    auto text_ptr = std::make_shared<TextChatMessage>();
    text_ptr->set_from_uid(12345);
    text_ptr->set_to_uid(67890);
    text_ptr->set_content("hello world");
    text_ptr->set_msg_type(TEXT);
    text_ptr->set_chat_msg_id("aaaaaaa");

//    auto send_msg_head = text_ptr->SerializeHeader();
    auto send_msg_body = text_ptr->Serialize();
    cout << "=============== send msg ================= " << endl;
//    cout << send_msg_head << endl;
    cout << send_msg_body << endl;
//    cout << send_msg_head + send_msg_body << endl;

    TextChatMessage text;
    text.Deserialize(send_msg_body);
    cout << "=============== receive msg ================= " << endl;
    cout << "from_uid: " << text.from_uid() << endl;
    cout << "to_uid: " << text.to_uid() << endl;
    cout << "error_codes: " << text.error_codes() << endl;
    cout << "message_id: " << text.message_id() << endl;
    cout << "timestamp: " << text.timestamp().to_seconds() << endl;
    cout << "content: " << text.content() << endl;
    cout << "msg_type: " << text.msg_type() << endl;
    cout << "chat_msg_id: " << text.chat_msg_id() << endl;

    return 0;
}