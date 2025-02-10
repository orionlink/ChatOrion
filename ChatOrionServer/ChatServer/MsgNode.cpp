//
// Created by hwk on 2025/1/14.
//

#include "MsgNode.h"
#include "const.h"

RecvNode::RecvNode(short max_len, short msg_id)
    :MsgNode(max_len), _msg_id(msg_id)
{

}

SendNode::SendNode(const char *msg, short max_len, short msg_id)
    :MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id)
{
    short msg_id_net = boost::asio::detail::socket_ops::host_to_network_short(msg_id);

    // 拷贝消息id
    ::memcpy(_data, &msg_id_net, HEAD_ID_LEN);

    //转为网络字节序
    short max_len_net = boost::asio::detail::socket_ops::host_to_network_short(max_len);
    // 拷贝消息长度
    ::memcpy(_data + HEAD_ID_LEN, &max_len_net, HEAD_DATA_LEN);
    // 拷贝消息体
    ::memcpy(_data + HEAD_ID_LEN + HEAD_DATA_LEN, msg, max_len);
}
