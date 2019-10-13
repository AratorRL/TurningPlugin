#pragma once
#include "Configuration.h"

class FreeTurnConfiguration : public Configuration
{
private:
    Rotator startRot;

public:
    FreeTurnConfiguration(GlobalObjects obj) : Configuration(obj) {};

    virtual void init();
    virtual void tick();
    virtual void end();
};