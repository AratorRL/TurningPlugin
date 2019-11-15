#pragma once
#pragma comment (lib, "bakkesmod.lib")

#include "utils.h"
#include "FreeTurnExercise.h"
#include "FixedTurnExercise.h"
#include "CustomTrainingTurnExercise.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

class TurningPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    virtual void onLoad();
    virtual void onUnload();

	FreeTurnExercise* freeTurnExercise;
	FixedTurnExercise* fixedTurnExercise;
	CustomTrainingTurnExercise* customTrainingTurnExercise;

	FreeTurnExercise* getFreeTurnExercise();
	FixedTurnExercise* getFixedTurnExercise();
	CustomTrainingTurnExercise* getCustomTrainingTurnExercise();

	TurnExercise* currentExercise;

private:
	void switchToExercise(TurnExercise* exercise);
};