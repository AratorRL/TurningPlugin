#include "Logger.h"

std::string vector_to_string(Vector v)
{
    return std::to_string(v.X) + ", " + std::to_string(v.Y) + ", " + std::to_string(v.Z);
}

std::string rot_to_string(Rotator r)
{
    return std::to_string(r.Pitch) + ", " + std::to_string(r.Yaw) + ", " + std::to_string(r.Roll);
}

void Logger::log(std::string s)
{
    cvarManager->log(s);
}

void Logger::logv(Vector v)
{
    cvarManager->log(vector_to_string(v));
}

void Logger::logr(Rotator r)
{
    cvarManager->log(rot_to_string(r));
}

void Logger::logf(float f)
{
    cvarManager->log(std::to_string(f));
}

void Logger::logb(bool b)
{
    cvarManager->log(std::to_string(b));
}

void Logger::logi(int i)
{
    cvarManager->log(std::to_string(i));
}

void Logger::logl(unsigned long l)
{
    cvarManager->log(std::to_string(l));
}