#pragma once
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

class Logger
{
public:
    std::shared_ptr<CVarManagerWrapper> cvarManager;

    Logger(std::shared_ptr<CVarManagerWrapper> cvarManager) : cvarManager(cvarManager) {};

    void log(std::string s);
    void logv(Vector v);
    void logr(Rotator r);
    void logf(float f);
    void logb(bool b);
    void logi(int i);
    void logl(unsigned long l);
};