#pragma once
#include "utils.h"
#include "Logger.h"
#include "bakkesmod/wrappers/GameWrapper.h"

class Configuration
{
public:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;

    Rotator acceptingState;

    Configuration(std::shared_ptr<GameWrapper> game, Logger* logger) : game(game), logger(logger) {};

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
    StationaryBallConfiguration(std::shared_ptr<GameWrapper> game, Logger* logger) : Configuration(game, logger) {};

    virtual void init();
    virtual void tick();
    virtual void end();
};