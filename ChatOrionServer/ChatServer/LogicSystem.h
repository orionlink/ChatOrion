//
// Created by hwk on 2025/1/14.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "singleton.hpp"
#include "CSession.h"

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();

    void postMsgToQue(std::shared_ptr<LogicNode> msg);
private:

};



#endif //LOGICSYSTEM_H
