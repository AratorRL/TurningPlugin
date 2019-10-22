#define _USE_MATH_DEFINES
#include <math.h>
#include "TurningExercise.h"
#include "utils.h"

TurningExercise::TurningExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	this->game = game;
	this->cvarManager = cvarManager;
}

void TurningExercise::init()
{
	cvarManager->log("Turning exercise init.");
	start();
}

void TurningExercise::start()
{
	this->isActive = true;

	// this->startRot = util::getCarRotation(game);
	// this->lastRot = startRot;
	this->goalRot = util::turnClockwise(startRot, 16384); 

	this->isTurning = false;
	this->ticksWithSameRot = 0;
	
	util::hookPhysicsTick(game, std::bind(&TurningExercise::tick, this));
}

void TurningExercise::tick()
{
	Rotator currentRot = util::getCarRotation(game);

	ControllerInput input = util::getCar(game).GetInput();
	if (!isTurning && input.Steer != 0)
	{
		isTurning = true;
		this->startRot = currentRot;
		this->lastRot = currentRot;
	}

	if (isTurning)
	{
		saveSnapshot();
	}

	if (isTurning && input.Steer == 0) // stopped turning
	{
		if (util::isInRotRange(lastRot, currentRot, 100))
		{
			ticksWithSameRot++;

			if (ticksWithSameRot > 60)
			{
				finalRot = currentRot;
				util::unhookPhysicsTick(game);
				end();
			}			
		}
		else
		{
			ticksWithSameRot = 0;
		}
	}	

	lastRot = currentRot;
}

void TurningExercise::end()
{
	cvarManager->log("Turning exercise end.");

	game->UnregisterDrawables();
	game->RegisterDrawable(std::bind(&TurningExercise::visualize, this, std::placeholders::_1), 0);

	if (util::isInRotRange(finalRot, goalRot, 2000))
	{
		cvarManager->log("Goal reached.");
	}

	this->isActive = false;
}

void TurningExercise::saveSnapshot()
{
	// cvarManager->log("saving input snapshot");

	CarWrapper car = util::getCar(game);
	Vector loc = car.GetLocation();
	Rotator rot = car.GetRotation();

	ControllerInput input = util::getCar(game).GetInput();

	this->recording.snapshots.push_back({
		loc,
		rot,
		input.Throttle,
		input.Steer,
		(bool)input.HoldingBoost,
		(bool)input.Handbrake
		});

	if (loc.X < recording.minX)
		recording.minX = loc.X;
	if (loc.X > recording.maxX)
		recording.maxX = loc.X;
	if (loc.Y < recording.minY)
		recording.minY = loc.Y;
	if (loc.Y > recording.maxY)
		recording.maxY = loc.Y;
}

Vector2 rotateVec2(Vector2F vec, float angle)
{
	int x = (int)((float)vec.X * cos(angle) - (float)vec.Y * sin(angle));
	int y = (int)((float)vec.X * sin(angle) + (float)vec.Y * cos(angle));
	return Vector2{ x, y };
}

Vector2 TurningExercise::getLocalCoordinate(TurningSnapshot snap, TurningRecording recording)
{
	int width = (recording.maxX - recording.minX);
	int height = (recording.maxY - recording.minY);

	// cvarManager->log("width: " + to_string(width) + ", height: " + to_string(height));

	float widthScale = (float)drawingWidth / (float)width;
	float heightScale = (float)drawingHeight / (float)height;
	float scale = min(widthScale, heightScale);

	float x = (float)(snap.location.X - recording.minX) * scale;
	float y = (float)(snap.location.Y - recording.minY) * scale;

	float angle = startRot.Yaw * M_PI / 32768;
	// cvarManager->log("angle: " + to_string(angle));
	Vector2 vec = rotateVec2(Vector2F{ x, y }, angle); // clockwise
	// cvarManager->log("vec: " + to_string(vec.X) + ", " + to_string(vec.Y));

	return Vector2{ drawingX + vec.X, drawingY + vec.Y };
}

void TurningExercise::drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end)
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

void TurningExercise::visualize(CanvasWrapper canvas)
{
	if (recording.snapshots.size() <= 0)
		return;

	// cvarManager->log("# snapshots found: " + to_string(recording.snapshots.size()));

	canvas.SetPosition(Vector2{ drawingX, drawingY });
	canvas.DrawBox(Vector2{ drawingWidth, drawingHeight });

	TurningSnapshot snap0 = recording.snapshots.at(0);
	Vector2 lastCoord = getLocalCoordinate(snap0, recording);

	float angle = startRot.Yaw * M_PI / 32768;
	// cvarManager->log("angle: " + to_string(angle));
	canvas.DrawString("angle: " + to_string(angle));

	canvas.SetColor(255, 0, 0, 255);

	for (int i = 1; i < recording.snapshots.size(); i++)
	{
		TurningSnapshot snap = recording.snapshots.at(i);
		Vector2 coord = getLocalCoordinate(snap, recording);

		drawThiccLine(canvas, lastCoord, coord);
		lastCoord = coord;
	}
}