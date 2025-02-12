//
// Created by hwk on 2025/1/14.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "singleton.hpp"
#include "CSession.h"
#include "const.h"
#include "data.h"
#include "BS_thread_pool.hpp"

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    using msg_handle_callback = std::function<void(std::shared_ptr<CSession>, const std::string &msg_data)>;

    ~LogicSystem();

    /**
     * 将消息推送到任务队列中，由线程池处理
     * @param msg 消息
     */
    void postMsgToQue(std::shared_ptr<LogicNode> msg);

    void registerMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler);
    void unregisterMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler);
private:
    LogicSystem();

    /**
     * 处理登录请求回调函数
     * @param session
     * @param msg_data
     */
    void LoginHandler(std::shared_ptr<CSession> session, const std::string &msg_data);

    /**
     * 处理搜索用户请求回调函数
     * @param session
     * @param msg_data
     */
    void SearchInfoHandler(std::shared_ptr<CSession> session, const std::string &msg_data);

    /**
     * 添加好友请求
     * @param session
     * @param msg_data
     */
    void AddFriendApplyHandler(std::shared_ptr<CSession> session, const std::string &msg_data);
private:
    /**
     * 判断字符串是否都是数字字符
     * @param str
     * @return
     */
    bool isPureDigit(const std::string& str);

    /**
     * 获取用户信息
     * @param uid [in]
     * @param userinfo [out]
     * @return
     */
    bool GetBaseInfo(int uid, std::shared_ptr<UserInfo>& userinfo);

    /**
     * 根据用户id查询返回用户信息并写入到 Json::Value 中
     * @param uid_str 用户id
     * @param rt_value 返回的json数据
     */
    void GetUserByUid(const std::string& uid_str, Json::Value& rt_value);

    /**
     * 根据用户名称查询返回用户信息并写入到 Json::Value 中
     * @param username_str
     * @param rt_value
     */
    void GetUserByName(const std::string& username_str, Json::Value& rt_value);
private:
    BS::thread_pool<BS::tp::pause> _pool;  // 线程池
    bool _b_stop;
    std::map<MSG_IDS, msg_handle_callback> _handle_callbacks;
};



#endif //LOGICSYSTEM_H
