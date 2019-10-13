#pragma once
#include "utils.h"
#include "Logger.h"
#include "bakkesmod/wrappers/GameWrapper.h"

class Configuration
{
public:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;
    Drawer* drawer;

    Rotator acceptingState;

    Configuration(GlobalObjects obj)
        : game(obj.game), logger(obj.logger), drawer(obj.drawer) {};

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void end() = 0;
    
    void setAcceptingState(Rotator rot);
    void hookPhysicsTick();
    void unhookPhysicsTick();
};