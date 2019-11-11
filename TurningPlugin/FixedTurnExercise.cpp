#define _USE_MATH_DEFINES
#include <math.h>
#include "FixedTurnExercise.h"
#include "utils.h"

FixedTurnExercise::FixedTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	this->game = game;
	this->cvarManager = cvarManager;

	this->recording[0] = new TurningRecording();
	this->recording[1] = new TurningRecording();
}

void FixedTurnExercise::init()
{
	cvarManager->log("Turning exercise init.");
	game->RegisterDrawable(std::bind(&FixedTurnExercise::visualize, this, std::placeholders::_1));
	util::hookPhysicsTick(game, std::bind(&FixedTurnExercise::tick, this));
	game->HookEventWithCallerPost<CarWrapper>("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&FixedTurnExercise::OnHitBall, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	this->recording[0]->reset();
	this->recording[1]->reset();
	reset();
}

void FixedTurnExercise::reset()
{
	CarWrapper car = util::getCar(game);
	car.Stop();
	car.GetDodgeComponent().SetActive(false);
	// car.SetLocation({ -1000, 1000, 50 });
	float x = cvarManager->getCvar("turn_fixed_x").getFloatValue();
	float y = cvarManager->getCvar("turn_fixed_y").getFloatValue();
	int rot = cvarManager->getCvar("turn_fixed_rot").getIntValue();
	int boost = cvarManager->getCvar("turn_fixed_boost").getIntValue();
	int goalYaw = cvarManager->getCvar("turn_fixed_goalrot").getIntValue();
	this->goalRange = cvarManager->getCvar("turn_fixed_goalrange").getIntValue();
	this->goalRot = { 0, goalYaw, 0 };
	// car.SetRotation({ 0, 0, 0 });
	car.SetLocation({ x, y, 50 });
	car.SetRotation({ 0, rot, 0 });

	car.GetBoostComponent().SetBoostAmount((float)(boost / 100.0));

	car.SetVelocity({ 0, 0, 0 });
	BallWrapper ball = util::getBall(game);
	ball.Stop();
	ball.SetLocation({ 0, 500, 80 });

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
		cvarManager->log("onhitBall");

		OnHitBallParms* parms = (OnHitBallParms*)params;
		Vector hitLoc = parms->HitLocation;

		CarWrapper car = util::getCar(game);
		Rotator finalRot = car.GetRotation();
		Vector carLoc = car.GetLocation();
		Vector ballLoc = util::getBall(game).GetLocation();

		Vector relativeLoc = ballLoc - carLoc;

		Rotator relativeRot = VectorToRotator(relativeLoc);

		if (util::isInRotRange(finalRot, goalRot, goalRange) && util::isInRotRange(relativeRot, goalRot, goalRange))
		{
			cvarManager->log("Goal reached.");
		}
		else
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

TurningRecording* FixedTurnExercise::getCurrentRecording()
{
	return this->recording[currRecordingBuffer];
}

TurningRecording* FixedTurnExercise::getLastRecording()
{
	return this->recording[1 - currRecordingBuffer];
}

void FixedTurnExercise::swapRecordingBuffers()
{
	this->currRecordingBuffer = 1 - this->currRecordingBuffer;
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
	cvarManager->log("Turning exercise end.");

	swapRecordingBuffers();
	analyzeTurn(this->getLastRecording());
	cvarManager->log("end of end");
	// this->isActive = false;
}

void FixedTurnExercise::clear()
{
	this->isActive = false;
	game->UnregisterDrawables();
	util::unhookPhysicsTick(game);
	game->UnhookEvent("Function TAGame.Car_TA.ApplyBallImpactForces");
}

void FixedTurnExercise::saveSnapshot()
{
	// cvarManager->log("saving input snapshot");

	CarWrapper car = util::getCar(game);
	Vector loc = car.GetLocation();
	Rotator rot = car.GetRotation();

	ControllerInput input = util::getCar(game).GetInput();

	TurningRecording* recording = this->getCurrentRecording();

	recording->snapshots.push_back({
		loc,
		rot,
		input.Throttle,
		input.Steer,
		(bool)input.HoldingBoost,
		(bool)input.Handbrake
		});
}

void FixedTurnExercise::analyzeTurn(TurningRecording* rec)
{
	cvarManager->log("analyze 1");
	cvarManager->log(to_string(rec->snapshots.size()));

	if (rec->snapshots.size() <= 0)
	{
		return;
	}
	
	TurningSnapshot firstSnap = rec->snapshots.front();
	float startAngle = -firstSnap.rotation.Yaw * M_PI / 32768 - M_PI / 2;

	int currentInput = INPUT_NONE;

	cvarManager->log("analyze 2");

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

	cvarManager->log("analyze 3");

	Vector2 firstVec = rec->points.front();
	Vector2 lastVec = rec->points.back();

	rec->isTurningLeft = lastVec.X < firstVec.X;
}

void FixedTurnExercise::drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			Vector2 s = { start.X + i, start.Y + j };
			Vector2 e = { end.X + i, end.Y + j };
			cw.DrawLine(s, e);
		}
	}
}

RGBA FixedTurnExercise::getColor(TurningSnapshot snap)
{
	if (snap.boost && snap.powerslide)
		return RGBA{ (char)255, (char)255, (char)0, (char)255 };
	else if (snap.boost)
		return RGBA{ (char)255, (char)0, (char)0, (char)255 };
	else if (snap.powerslide)
		return RGBA{ (char)0, (char)255, (char)0, (char)255 };
	else
		return RGBA{ (char)255, (char)255, (char)255, (char)255 };
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

		float angle = this->goalRange * M_PI / 32768;
		float x = end.X * cos(angle) - (float)end.Y * sin(angle);
		float y = end.X * sin(angle) + (float)end.Y * cos(angle);

		Vector end1 = start + Vector{ y, x, 0 };
		Vector end2 = start + Vector{ -y, x, 0 };

		Vector2 end1Proj = canvas.Project(end1);
		Vector2 end2Proj = canvas.Project(end2);
		Vector2 startProj = canvas.Project(start);

		canvas.SetColor(255, 0, 0, 255);

		drawThiccLine(canvas, startProj, end1Proj);
		drawThiccLine(canvas, startProj, end2Proj);
	}
	
	
	TurningRecording* recording = this->getLastRecording();

	if (recording->snapshots.size() <= 0)
	{
		//cvarManager->log("no snapshots in recording");
		return;
	}

	// canvas.SetPosition(Vector2{ 100, 100 });
	// canvas.SetColor(255, 0, 0, 255);
	// canvas.DrawString("# segments: " + to_string(recording->segments.size()));

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

		RGBA color = getColor(recording->snapshots.at(i));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		drawThiccLine(canvas, lastCoord, coord);
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

		RGBA color = getColor(recording->snapshots.at(middleIndex));
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