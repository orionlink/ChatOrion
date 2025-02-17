//
// Created by hwk on 2025/1/14.
//

#ifndef MSGNODE_H
#define MSGNODE_H

#include <cstring>
#include <string>
#include <iostream>

class MsgNode
{
public:
    MsgNode(short max_len) : _total_len(max_len), _current_len(0)
    {
        _data = new char[_total_len + 1]();
        _data[_total_len] = '\0';
    }

    ~MsgNode()
    {
        std::cout << "destruct MsgNode" << std::endl;
        delete[] _data;
    }

    void clear() {
        ::memset(_data, 0, _total_len);
        _current_len = 0;
    }

    short _current_len;
    short _total_len;
    char* _data;
};

class RecvNode :public MsgNode
{
    friend class LogicSystem;
public:
    RecvNode(short max_len, short msg_id);
private:
    short _msg_id;
};

class SendNode:public MsgNode
{
    friend class LogicSystem;
public:
    SendNode(const char* msg,short max_len, short msg_id);
private:
    short _msg_id;
};

#endif //MSGNODE_H
