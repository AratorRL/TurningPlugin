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

void Logger::log(Vector v)
{
    cvarManager->log(vector_to_string(v));
}

void Logger::log(Rotator r)
{
    cvarManager->log(rot_to_string(r));
}

void Logger::log(float f)
{
    cvarManager->log(std::to_string(f));
}

void Logger::log(bool b)
{
    cvarManager->log(std::to_string(b));
}

void Logger::log(int i)
{
    cvarManager->log(std::to_string(i));
}

void Logger::log(unsigned long l)
{
    cvarManager->log(std::to_string(l));
}