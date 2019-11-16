#define _USE_MATH_DEFINES
#include <math.h>
#include "FixedTurnExercise.h"
#include "utils.h"
#include "Recording.h"

FixedTurnExercise::FixedTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	this->game = game;
	this->cvarManager = cvarManager;

	this->recording[0] = new TurningRecording();
	this->recording[1] = new TurningRecording();
}

void FixedTurnExercise::init()
{
	game->RegisterDrawable(std::bind(&FixedTurnExercise::visualize, this, std::placeholders::_1));
	util::hookPhysicsTick(game, std::bind(&FixedTurnExercise::tick, this));
	game->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&FixedTurnExercise::OnHitBall, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	this->recording[0]->reset();
	this->recording[1]->reset();
	reset();
}

void FixedTurnExercise::reset()
{
	CarWrapper car = util::getCar(game);
	car.Stop();
	car.GetDodgeComponent().SetActive(false);
	car.SetVelocity({ 0, 0, 0 });

	BallWrapper ball = util::getBall(game);
	ball.Stop();

	float x = cvarManager->getCvar("turn_fixed_x").getFloatValue();
	float y = cvarManager->getCvar("turn_fixed_y").getFloatValue();
	car.SetLocation({ x, y, 50 });

	float yaw = cvarManager->getCvar("turn_fixed_rot").getFloatValue();
	int unrealYaw = (int)((yaw - 90) / 180 * 32768);
	Rotator rot = Rotator{ 0, unrealYaw, 0 };
	car.SetRotation(rot);

	float carSpeed = cvarManager->getCvar("turn_fixed_carspeed").getFloatValue();
	Vector forward = RotatorToVector(rot);
	car.SetVelocity({ forward.X * carSpeed, forward.Y * carSpeed, forward.Z * carSpeed });

	int boost = cvarManager->getCvar("turn_fixed_boost").getIntValue();
	car.GetBoostComponent().SetBoostAmount((float)(boost / 100.0));

	float targetYaw = cvarManager->getCvar("turn_fixed_targetrot").getFloatValue();
	int unrealTargetYaw = (int)((targetYaw - 90) / 180 * 32768);
	this->targetRot = { 0, unrealTargetYaw, 0 };
	
	float targetMargin = cvarManager->getCvar("turn_fixed_targetmargin").getFloatValue();
	this->targetMargin = (int)((targetMargin) / 180 * 32768);

	ball.SetLocation({ 0, 0, 92.74 });
	
	float ballSpeed = cvarManager->getCvar("turn_fixed_ballspeed").getFloatValue();
	ball.SetVelocity({ 0, ballSpeed, 0 });

	ball.SetbMovable(true);
	car.SetbMovable(true);

	// this->isTurning = false;
	// this->ticksWithSameRot = 0;
	this->currRecordingBuffer = 0;

	this->isActive = true;
	this->getCurrentRecording()->reset();
	this->hitBall = false;
	this->isDriving = false;
	this->hasJustReset = true;
}

struct OnHitBallParms
{
	void* ptr;
	Vector HitLocation;
	Vector HitNormal;
};

void FixedTurnExercise::OnHitBall(CarWrapper caller, void* params, std::string eventName)
{
	if (this->isActive)
	{
		OnHitBallParms* parms = (OnHitBallParms*)params;
		Vector hitLoc = parms->HitLocation;

		CarWrapper car = util::getCar(game);
		Rotator finalRot = car.GetRotation();
		Vector carLoc = car.GetLocation();
		Vector ballLoc = util::getBall(game).GetLocation();

		Vector relativeLoc = ballLoc - carLoc;

		Rotator relativeRot = VectorToRotator(relativeLoc);

		bool freeze = cvarManager->getCvar("turn_fixed_freeze").getBoolValue();

		cvarManager->log("final rot: " + to_string(finalRot.Yaw));
		cvarManager->log("target rot: " + to_string(targetRot.Yaw));

		int currYaw = finalRot.Yaw;
		int goalYaw = targetRot.Yaw;

		// make the difference is < 65536
		while (abs(currYaw - goalYaw) >= 32768)
		{
			if (currYaw < goalYaw)
				currYaw += 65536;
			else
				goalYaw += 65536;
		}

		cvarManager->log("currYaw: " + to_string(currYaw));
		cvarManager->log("goalYaw: " + to_string(goalYaw));

		if (freeze && (!util::isInRotRange(finalRot, targetRot, targetMargin) || !util::isInRotRange(relativeRot, targetRot, targetMargin)))
		{
			freezeAll();
		}
		
		this->hitBall = true;
		this->isActive = false;
		end();
	}
}

void FixedTurnExercise::freezeAll()
{
	CarWrapper car = util::getCar(game);
	BallWrapper ball = util::getBall(game);

	car.SetVelocity({ 0, 0, 0 });
	car.SetAngularVelocity({ 0, 0, 0 }, false);

	ball.SetVelocity({ 0, 0, 0 });
	ball.SetAngularVelocity({ 0, 0, 0 }, false);

	ball.SetbMovable(false);
	car.SetbMovable(false);
}

void FixedTurnExercise::tick()
{
	if (this->isActive)
	{
		if (this->hasJustReset)
		{
			this->hasJustReset = false;
			return;
		}
		
		ControllerInput input = util::getCar(game).GetInput();
		this->isDriving = input.Throttle != 0.0;

		if (/*this->isDriving && */!this->hitBall)
		{
			saveSnapshot();
		}
	}
}

void FixedTurnExercise::end()
{
	swapRecordingBuffers();
	analyzeTurn(this->getLastRecording());
}

void FixedTurnExercise::clear()
{
	this->isActive = false;
	game->UnregisterDrawables();
	util::unhookPhysicsTick(game);
	game->UnhookEvent("Function TAGame.Car_TA.ApplyBallImpactForces");

	CarWrapper car = util::getCar(game);
	BallWrapper ball = util::getBall(game);
	car.SetbMovable(true);
	ball.SetbMovable(true);
}

void FixedTurnExercise::visualize(CanvasWrapper canvas)
{
	if (this->isActive && !this->hitBall)
	{
		// draw indicator lines on the ground
		BallWrapper ball = util::getBall(game);
		Vector start = ball.GetLocation();
		start.Z = 0;
		Vector end = { 200, 0, 0 };

		float angle1 = -1 * ((float)(this->targetRot.Yaw + this->targetMargin) * M_PI / 32768 + (M_PI / 2));
		float angle2 = -1 * ((float)(this->targetRot.Yaw - this->targetMargin) * M_PI / 32768 + (M_PI / 2));
		float x1 = end.X * cos(angle1) - (float)end.Y * sin(angle1);
		float y1 = end.X * sin(angle1) + (float)end.Y * cos(angle1);
		float x2 = end.X * cos(angle2) - (float)end.Y * sin(angle2);
		float y2 = end.X * sin(angle2) + (float)end.Y * cos(angle2);

		Vector end1 = start + Vector{ y1, x1, 0 };
		Vector end2 = start + Vector{ y2, x2, 0 };

		Vector2 end1Proj = canvas.Project(end1);
		Vector2 end2Proj = canvas.Project(end2);
		Vector2 startProj = canvas.Project(start);

		canvas.SetColor(255, 0, 0, 255);

		util::drawThiccLine(canvas, startProj, end1Proj);
		util::drawThiccLine(canvas, startProj, end2Proj);
	}
	
	TurnExercise::visualize(canvas);
}