#pragma once
#pragma comment (lib, "bakkesmod.lib")

#include "Drawer.h"
#include "Logger.h"
#include "Configuration.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

class TurningPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad();
    virtual void onUnload();

private:
    Drawer* drawer;
    Logger* logger;
    Configuration* currentConfig;

    void hookPhysicsTick(std::function<void(std::string eventName)> callback);
    void turningTest();
    void init();
    void OnHit();
    void freezeAll();
};