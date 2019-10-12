#pragma once
#include "utils.h"
#include "Logger.h"
#include "Visualizer.h"
#include "bakkesmod/wrappers/GameWrapper.h"

class Configuration
{
public:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;
    Visualizer* visualizer;

    Rotator acceptingState;

    Configuration(GlobalObjects obj) 
        : game(obj.game), logger(obj.logger) 
    {
        visualizer = new Visualizer(obj.game, obj.drawer);
    };

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void end() = 0;
    
    void setAcceptingState(Rotator rot);
    void hookPhysicsTick();
    void unhookPhysicsTick();
};

class StationaryBallConfiguration : public Configuration
{
public:
    StationaryBallConfiguration(GlobalObjects obj) : Configuration(obj) {};

    virtual void init();
    virtual void tick();
    virtual void end();
};