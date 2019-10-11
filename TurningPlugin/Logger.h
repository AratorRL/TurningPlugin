#pragma once
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

class Logger
{
public:
    std::shared_ptr<CVarManagerWrapper> cvarManager;

    Logger(std::shared_ptr<CVarManagerWrapper> cvarManager) : cvarManager(cvarManager) {};

    void log(std::string s);
    void log(Vector v);
    void log(Rotator r);
    void log(float f);
    void log(bool b);
    void log(int i);
    void log(unsigned long l);
};