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

	int yaw = cvarManager->getCvar("turn_fixed_rot").getIntValue();
	Rotator rot = Rotator{ 0, yaw, 0 };
	car.SetRotation(rot);

	float carSpeed = cvarManager->getCvar("turn_fixed_carspeed").getFloatValue();
	Vector forward = RotatorToVector(rot);
	car.SetVelocity({ forward.X * carSpeed, forward.Y * carSpeed, forward.Z * carSpeed });

	int boost = cvarManager->getCvar("turn_fixed_boost").getIntValue();
	car.GetBoostComponent().SetBoostAmount((float)(boost / 100.0));

	int targetYaw = cvarManager->getCvar("turn_fixed_targetrot").getIntValue();
	this->targetRot = { 0, targetYaw, 0 };
	this->targetMargin = cvarManager->getCvar("turn_fixed_targetmargin").getIntValue();

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

		if (!util::isInRotRange(finalRot, targetRot, targetMargin) || !util::isInRotRange(relativeRot, targetRot, targetMargin))
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

		if (this->isDriving && !this->hitBall)
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

void FixedTurnExercise::analyzeTurn(TurningRecording* rec)
{
	if (rec->snapshots.size() <= 0)
		return;
	
	TurningSnapshot firstSnap = rec->snapshots.front();
	float startAngle = -firstSnap.rotation.Yaw * M_PI / 32768 - M_PI / 2;

	int currentInput = INPUT_NONE;

	for (int i = 0; i < rec->snapshots.size(); i++)
	{
		TurningSnapshot snap = rec->snapshots.at(i);
		Vector relativeLoc = snap.location - firstSnap.location;
		Vector2 vec = util::rotateVec2(Vector2F{ relativeLoc.X, relativeLoc.Y }, startAngle);
		rec->points.push_back(vec);

		if (vec.X < rec->pbound.minX)
			rec->pbound.minX = vec.X;
		if (vec.X > rec->pbound.maxX)
			rec->pbound.maxX = vec.X;
		if (vec.Y < rec->pbound.minY)
			rec->pbound.minY = vec.Y;
		if (vec.Y > rec->pbound.maxY)
			rec->pbound.maxY = vec.Y;

		int input = INPUT_NONE;
		if (snap.boost)
			input += INPUT_BOOST;
		if (snap.powerslide)
			input += INPUT_POWERSLIDE;
		if (snap.throttle)
			input += INPUT_THROTTLE;

		if (input != currentInput)
		{
			rec->segments.push_back(TurningSegment{ i, input });
		}
		currentInput = input;
	}

	Vector2 firstVec = rec->points.front();
	Vector2 lastVec = rec->points.back();

	rec->isTurningLeft = lastVec.X < firstVec.X;
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

		float angle = this->targetMargin * M_PI / 32768;
		float x = end.X * cos(angle) - (float)end.Y * sin(angle);
		float y = end.X * sin(angle) + (float)end.Y * cos(angle);

		Vector end1 = start + Vector{ y, x, 0 };
		Vector end2 = start + Vector{ -y, x, 0 };

		Vector2 end1Proj = canvas.Project(end1);
		Vector2 end2Proj = canvas.Project(end2);
		Vector2 startProj = canvas.Project(start);

		canvas.SetColor(255, 0, 0, 255);

		util::drawThiccLine(canvas, startProj, end1Proj);
		util::drawThiccLine(canvas, startProj, end2Proj);
	}
	
	
	TurningRecording* recording = this->getLastRecording();

	if (recording->snapshots.size() <= 0)
		return;
	
	canvas.SetPosition(Vector2{ drawingX, drawingY });
	canvas.SetColor(100, 100, 100, 100);
	canvas.DrawBox(Vector2{ drawingWidth, drawingHeight });

	Vector2 origin = Vector2{ drawingX, drawingY + drawingHeight };
	if (recording->isTurningLeft)
		origin.X += drawingWidth;

	int width = recording->pbound.maxX - recording->pbound.minX;
	int height = recording->pbound.maxY - recording->pbound.minY;
	float scale = (float)max(drawingWidth, drawingHeight) / (float)max(width, height);

	Vector2 lastPoint = recording->points.front();
	Vector2 lastCoord = origin;

	for (int i = 1; i < recording->points.size(); i++)
	{
		Vector2 point = recording->points.at(i);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };

		RGBA color = getSnapshotColor(recording->snapshots.at(i));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		util::drawThiccLine(canvas, lastCoord, coord);
		lastCoord = coord;
	}
	
	for (int i = 0; i < recording->segments.size(); i++)
	{
		TurningSegment seg = recording->segments.at(i);
		int nextIndex;
		if (i < recording->segments.size() - 1)
			nextIndex = recording->segments.at(i + 1).startIndex;
		else
			nextIndex = recording->points.size() - 1;

		int middleIndex = (seg.startIndex + nextIndex) / 2;

		RGBA color = getSnapshotColor(recording->snapshots.at(middleIndex));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		Vector2 point = recording->points.at(middleIndex);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };
		if (recording->isTurningLeft)
			coord.X += 20;
		else
			coord.X -= 20;

		canvas.SetPosition(coord);
		canvas.DrawString(to_string(nextIndex - seg.startIndex), 1.0, 1.0);
	}
}