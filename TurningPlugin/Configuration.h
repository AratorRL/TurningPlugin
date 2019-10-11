#pragma once
#include "Logger.h"
#include "bakkesmod/wrappers/GameWrapper.h"

class Configuration
{
public:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;

    Configuration(std::shared_ptr<GameWrapper> game, Logger* logger) : game(game), logger(logger) {};

    virtual void init() = 0;
};

class StationaryBallConfiguration : public Configuration
{
public:
    StationaryBallConfiguration(std::shared_ptr<GameWrapper> game, Logger* logger) : Configuration(game, logger) {};

    virtual void init();
};