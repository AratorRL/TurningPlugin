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

void TurningPlugin::switchToExercise(TurnExercise* exercise)
{
	if (currentExercise)
	{
		currentExercise->clear();
	}
	currentExercise = exercise;
	currentExercise->init();
}

void TurningPlugin::onLoad()
{
    cvarManager->log("Turning Plugin loaded.");

	cvarManager->registerNotifier("turn_start_free", [this](std::vector<std::string>) {
		switchToExercise(getFreeTurnExercise());
    }, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_start_fixed", [this](std::vector<std::string>) {
		switchToExercise(getFixedTurnExercise());
	}, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_start_customtraining", [this](std::vector<std::string>) {
		switchToExercise(getCustomTrainingTurnExercise());
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

	cvarManager->registerNotifier("turn_clear_exercise", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
			currentExercise = NULL;
		}
	}, "", PERMISSION_ALL);

	gameWrapper->HookEvent("Function GameEvent_TrainingEditor_TA.Countdown.BeginState", [this](std::string eventName) {
		cvarManager->log("custom training init");
		if (cvarManager->getCvar("turn_customtraining").getBoolValue())
		{
			cvarManager->executeCommand("turn_start_customtraining");
		}
	});

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", [this](std::string eventName) {
		cvarManager->getCvar("turn_mode").setValue("off");
	});

	cvarManager->registerCvar("turn_mode", "off", "Turning plugin mode", true, false, 0, false, 0, true)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		std::string newValue = cvar.getStringValue();

		if (newValue == "free")
		{
			cvarManager->executeCommand("turn_start_free");
		}
		else if (newValue == "fixed")
		{
			cvarManager->executeCommand("turn_start_fixed");
		}
		else if (newValue == "off")
		{
			cvarManager->executeCommand("turn_clear_exercise");
		}
	});

	cvarManager->registerCvar("turn_fixed_x", "-1000", "X coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_y", "-500", "Y coord of starting position relative to the ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_rot", "160", "Starting orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_boost", "100", "Starting boost amount", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_targetrot", "0", "Target orientation", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_targetmargin", "20", "Target orientation margin", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_ballspeed", "500", "Starting speed of ball", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_carspeed", "500", "Starting speed of car", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_fixed_freeze", "1", "Freeze the car and ball if ball not approached with the correct angle", true, false, 0, false, 0, true);

	cvarManager->registerCvar("turn_graph_scale", "200", "Size of turning graph", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_x", "300", "X coordinate of turning graph", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_y", "380", "Y coordinate of turning graph", true, false, 0, false, 0, true);

	cvarManager->registerCvar("turn_free_straight_treshold", "30", "In free turn mode, number of ticks of driving straight before turn is considered to be finished", true, false, 0, false, 0, true);
	
	cvarManager->registerCvar("turn_customtraining", "0", "Show turning graph after ball hit in custom training", true, false, 0, false, 0, true)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		bool newValue = cvar.getBoolValue();
		if (!newValue)
		{
			cvarManager->executeCommand("turn_clear_exercise");
		}
	});


	cvarManager->registerCvar("turn_graph_boost", "0", "Let color depend on boost being held or not", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_powerslide", "0", "Let color depend on powerslide being held or not", true, false, 0, false, 0, true);
	cvarManager->registerCvar("turn_graph_steer", "0", "Let color depend on steering value being 0 or not", true, false, 0, false, 0, true);

	// update plugins tab
	cvarManager->executeCommand("cl_settings_refreshplugins");
	

}

void TurningPlugin::onUnload()
{
    cvarManager->log("Turning Plugin unloaded.");
}