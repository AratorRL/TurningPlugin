#pragma once
#pragma comment (lib, "bakkesmod.lib")

#include "utils.h"
#include "Logger.h"
#include "FreeTurnExercise.h"
#include "FixedTurnExercise.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

class TurningPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad();
    virtual void onUnload();

	FreeTurnExercise* freeTurnExercise;
	FixedTurnExercise* fixedTurnExercise;

	FreeTurnExercise* getFreeTurnExercise();
	FixedTurnExercise* getFixedTurnExercise();

	TurningExercise* currentExercise;
	Logger* logger;

private:
    void turningTest();
    void OnHit();
    void freezeAll();
};