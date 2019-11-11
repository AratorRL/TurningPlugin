#include "TurningPlugin.h"
#include "utils.h"

#include "bakkesmod/wrappers/wrapperstructs.h";

BAKKESMOD_PLUGIN(TurningPlugin, "Turning Plugin", "1.0", PERMISSION_OFFLINE);


// get singleton FreeTurnExercise object
FreeTurnExercise* TurningPlugin::getFreeTurnExercise()
{
	if (!freeTurnExercise)
	{
		freeTurnExercise = new FreeTurnExercise(gameWrapper, cvarManager);
	}
	return freeTurnExercise;
}

// get singleton FixedTurnExercise object
FixedTurnExercise* TurningPlugin::getFixedTurnExercise()
{
	if (!fixedTurnExercise)
	{
		fixedTurnExercise = new FixedTurnExercise(gameWrapper, cvarManager);
	}
	return fixedTurnExercise;
}

// get singleton CustomTrainingTurnExercise object
CustomTrainingTurnExercise* TurningPlugin::getCustomTrainingTurnExercise()
{
	if (!customTrainingTurnExercise)
	{
		customTrainingTurnExercise = new CustomTrainingTurnExercise(gameWrapper, cvarManager);
	}
	return customTrainingTurnExercise;
}

void TurningPlugin::onLoad()
{
    cvarManager->log("Turning Plugin loaded.");

    cvarManager->registerNotifier("turning_test", std::bind(&TurningPlugin::turningTest, this), "", PERMISSION_FREEPLAY);
    
	cvarManager->registerNotifier("turn_mode_freeturn", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
		}
		currentExercise = getFreeTurnExercise();
		cvarManager->log("currentExercise set to FreeTurnExercise");
		currentExercise->init();
    }, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_mode_fixedturn", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
		}
		currentExercise = getFixedTurnExercise();
		currentExercise->init();
	}, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_mode_customtraining", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
		}
		currentExercise = getCustomTrainingTurnExercise();
		currentExercise->init();
		}, "", PERMISSION_CUSTOM_TRAINING);

	cvarManager->registerNotifier("turn_reset", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->reset();
		}
		else
		{
			cvarManager->log("No exercise initialized.");
		}
	}, "", PERMISSION_FREEPLAY);


	cvarManager->registerCvar("turn_fixed_x", "-1000", "X coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_y", "-500", "Y coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_rot", "15000", "Starting orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_boost", "100", "Starting boost amount", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_goalrot", "-16384", "Goal orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_goalrange", "3000", "Goal orientation range", true, false, 0, false, 0, true);

    logger = new Logger(cvarManager);

}

void TurningPlugin::onUnload()
{
    cvarManager->log("Turning Plugin unloaded.");
}

void TurningPlugin::turningTest()
{
    cvarManager->log("turning test");

    if (!gameWrapper->IsInFreeplay())
        return;

    ServerWrapper game = gameWrapper->GetGameEventAsServer();

    CarWrapper car = game.GetGameCar();
    Vector carLoc = car.GetLocation();
    cvarManager->log(util::vecToString(carLoc));

    BallWrapper ball = game.GetBall();
    Vector ballLoc = ball.GetLocation();
    cvarManager->log(util::vecToString(ballLoc));
}