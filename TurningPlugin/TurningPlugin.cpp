#include "TurningPlugin.h"
#include "utils.h"
#include "StationaryBallConfiguration.h"
#include "FreeTurnConfiguration.h"

#include "bakkesmod/wrappers/wrapperstructs.h";

BAKKESMOD_PLUGIN(TurningPlugin, "Turning Plugin", "1.0", PLUGINTYPE_FREEPLAY);

void TurningPlugin::init()
{
    if (currentConfig)
    {
        currentConfig->init();
        monitor->startRecordingInput();
    }
    else
    {
        glob.logger->log("No configuration defined.");
    }
}


void TurningPlugin::onLoad()
{
    cvarManager->log("Turning Plugin loaded.");

    cvarManager->registerNotifier("turning_test", std::bind(&TurningPlugin::turningTest, this), "", PERMISSION_FREEPLAY);
    cvarManager->registerNotifier("turning_init", std::bind(&TurningPlugin::init, this), "", PERMISSION_FREEPLAY);
    cvarManager->registerNotifier("turning_freeturn", [this](std::vector<std::string>) {
        this->currentConfig = new FreeTurnConfiguration(glob);
    }, "", PERMISSION_FREEPLAY);

    glob.logger = new Logger(cvarManager);
    glob.drawer = new Drawer(gameWrapper, glob.logger);    
    glob.game = this->gameWrapper;

    gameWrapper->RegisterDrawable(std::bind(&Drawer::draw, glob.drawer, std::placeholders::_1), 0);
    /*Drawer* drawer2 = new Drawer(gameWrapper, glob.logger);
    drawer2->test = true;
    gameWrapper->RegisterDrawable(std::bind(&Drawer::draw, drawer2, std::placeholders::_1), 1);*/

    // currentConfig = new StationaryBallConfiguration(glob);
    // currentConfig->setAcceptingState({ 0, 0, 0 });
    currentConfig = new FreeTurnConfiguration(glob);
    monitor = new Monitor(gameWrapper, glob.logger, (FreeTurnConfiguration*)currentConfig);


    // gameWrapper->HookEvent("Function TAGame.Ball_TA.OnRigidBodyCollision", std::bind(&TurningPlugin::OnHit, this));
    // gameWrapper->HookEvent("Function TAGame.Car_TA.OnHitBall", std::bind(&TurningPlugin::OnHit, this));
    // gameWrapper->HookEvent("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&TurningPlugin::OnHit, this));
    gameWrapper->HookEventPost("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&TurningPlugin::OnHit, this));
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