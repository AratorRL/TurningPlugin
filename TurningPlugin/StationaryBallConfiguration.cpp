#include "StationaryBallConfiguration.h"

void StationaryBallConfiguration::init()
{
    logger->log("initializing car and ball");

    if (!game->IsInFreeplay())
        return;

    ServerWrapper server = game->GetGameEventAsServer();

    CarWrapper car = server.GetGameCar();
    BallWrapper ball = server.GetBall();

    ball.SetVelocity({ 0, 0, 0 });
    ball.SetRotation({ 0, 0, 0 });
    ball.SetAngularVelocity({ 0, 0, 0 }, false);
    ball.SetLocation({ -500, -3800, 90 });

    car.SetVelocity({ 0, 0, 0 });
    car.SetLocation({ -1100, -4300, 200 });
    car.SetAngularVelocity({ 0, 0, 0 }, false);
    car.SetRotation({ -179, 12525, 0 });

    car.GetDodgeComponent().SetActive(false);

    hookPhysicsTick();
}

void StationaryBallConfiguration::tick()
{
    Rotator currentRot = util::getCarRotation(game);

    if (util::isInRotRange(currentRot, acceptingState, 2000))
    {
        //logger->log("ACCEPT");
        end();
    }
    else
    {
        //logger->log("NO ACCEPT");
    }

}

void StationaryBallConfiguration::end()
{
    unhookPhysicsTick();
    visualizer->render();
    logger->log("end");
}