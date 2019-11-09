#include "TurningPlugin.h"
#include "utils.h"
#include "FreeTurnExercise.h"
#include "FixedTurnExercise.h"

#include "bakkesmod/wrappers/wrapperstructs.h";

BAKKESMOD_PLUGIN(TurningPlugin, "Turning Plugin", "1.0", PLUGINTYPE_FREEPLAY);


FreeTurnExercise* TurningPlugin::getFreeTurnExercise()
{
	if (!freeTurnExercise)
	{
		freeTurnExercise = new FreeTurnExercise(gameWrapper, cvarManager, FreeTurn);
	}
	return freeTurnExercise;
}

FixedTurnExercise* TurningPlugin::getFixedTurnExercise()
{
	if (!fixedTurnExercise)
	{
		fixedTurnExercise = new FixedTurnExercise(gameWrapper, cvarManager, FixedStart);
	}
	return fixedTurnExercise;
}

void TurningPlugin::onLoad()
{
    cvarManager->log("Turning Plugin loaded.");

    cvarManager->registerNotifier("turning_test", std::bind(&TurningPlugin::turningTest, this), "", PERMISSION_FREEPLAY);
    
	cvarManager->registerNotifier("turn_start_freeturn", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
		}
		currentExercise = getFreeTurnExercise();
		currentExercise->init();
    }, "", PERMISSION_FREEPLAY);

	cvarManager->registerNotifier("turn_start_fixedturn", [this](std::vector<std::string>) {
		if (currentExercise)
		{
			currentExercise->clear();
		}
		currentExercise = getFixedTurnExercise();
		currentExercise->init();
	}, "", PERMISSION_FREEPLAY);

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

    logger = new Logger(cvarManager);
    drawer = new Drawer(gameWrapper, logger);    
    
    // gameWrapper->HookEventPost("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&TurningPlugin::OnHit, this));
}

void TurningPlugin::OnHit()
{
    if (!gameWrapper->IsInFreeplay())
        return;

    cvarManager->log("BOOMER");
    ServerWrapper game = gameWrapper->GetGameEventAsServer();

    CarWrapper car = game.GetGameCar();
    BallWrapper ball = game.GetBall();

    Vector diff = ball.GetLocation() - car.GetLocation();
    Rotator orient = VectorToRotator(diff);
    int yaw = orient.Yaw;

    Rotator carOrient = car.GetRotation();
    int carYaw = carOrient.Yaw;

    // cvarManager->log(rotToString(orient));
    cvarManager->log(to_string(orient.Yaw));

    int range = 2000;

    if (yaw < -16384 - range || yaw > -16384 + range)
    {
        freezeAll();
    }

    if (carYaw < -16384 - range || carYaw > -16384 + range)
    {
        freezeAll();
    }
}

void TurningPlugin::freezeAll()
{
    if (!gameWrapper->IsInFreeplay())
        return;

    ServerWrapper game = gameWrapper->GetGameEventAsServer();

    CarWrapper car = game.GetGameCar();
    BallWrapper ball = game.GetBall();

    car.SetVelocity({ 0, 0, 0 });
    car.SetAngularVelocity({ 0, 0, 0 }, false);

    ball.SetVelocity({ 0, 0, 0 });
    ball.SetAngularVelocity({ 0, 0, 0 }, false);

    // ball.SetbMovable(false);
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