#pragma once
#include "Configuration.h"

class StationaryBallConfiguration : public Configuration
{
public:
    StationaryBallConfiguration(GlobalObjects obj) : Configuration(obj) {};

    virtual void init();
    virtual void tick();
    virtual void end();
};