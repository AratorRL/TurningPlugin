#pragma once
#pragma comment (lib, "bakkesmod.lib")

#include "utils.h"
#include "Drawer.h"
#include "Logger.h"
#include "Exercise.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

class TurningPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad();
    virtual void onUnload();

	Exercise* currentExercise;
	Logger* logger;
	Drawer* drawer;

private:
    void turningTest();
    void OnHit();
    void freezeAll();
};