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
		cvarManager->log("switching to customtraining");
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
			cvarManager->log("No active exercise.");
		}
	}, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_mode_off", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
			currentExercise = NULL;
		}
	}, "", PERMISSION_ALL);


	/*gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", [this](std::string eventName) {
		cvarManager->log("customtraining destroyed");
	});*/

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", [this](std::string eventName) {
		cvarManager->log("freeplay destroyed");
		if (currentExercise)
		{
			currentExercise->clear();
			currentExercise = NULL;
		}
	});


	cvarManager->registerCvar("turn_fixed_x", "-1000", "X coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_y", "-500", "Y coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_rot", "15000", "Starting orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_boost", "100", "Starting boost amount", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_targetrot", "-16384", "Target orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_targetmargin", "3000", "Target orientation margin", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_ballspeed", "500", "Starting speed of ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_carspeed", "500", "Starting speed of car", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_scale", "200", "Size of turning graph", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_x", "300", "X coordinate of turning graph", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_y", "380", "Y coordinate of turning graph", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_free_straight_treshold", "30", "In free turn mode, number of ticks of driving straight before turn is considered to be finished", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_freeze", "1", "Freeze the car and ball if ball not approached with the correct angle", true, false, 0, false, 0, true);

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