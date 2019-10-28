#define _USE_MATH_DEFINES
#include <math.h>
#include "TurningExercise.h"
#include "utils.h"

TurningExercise::TurningExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	this->game = game;
	this->cvarManager = cvarManager;

	this->recording[0] = new TurningRecording();
	this->recording[1] = new TurningRecording();
}

void TurningExercise::init()
{
	cvarManager->log("Turning exercise init.");
	game->RegisterDrawable(std::bind(&TurningExercise::visualize, this, std::placeholders::_1));
	util::hookPhysicsTick(game, std::bind(&TurningExercise::tick, this));
	start();
}

void TurningExercise::start()
{
	this->isActive = true;

	// this->startRot = util::getCarRotation(game);
	// this->lastRot = startRot;
	// this->goalRot = util::turnClockwise(startRot, 16384); 

	this->isTurning = false;
	this->ticksWithSameRot = 0;
	this->recording[0]->reset();
	this->recording[1]->reset();
	this->currRecordingBuffer = 0;
}

TurningRecording* TurningExercise::getCurrentRecording()
{
	return this->recording[currRecordingBuffer];
}

TurningRecording* TurningExercise::getLastRecording()
{
	return this->recording[1 - currRecordingBuffer];
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
		this->goalRot = util::turnClockwise(startRot, 16384);
		isActive = true;
		this->ticksWithSameRot = 0;
		this->getCurrentRecording()->reset();
		cvarManager->log("Start turning.");
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
				isTurning = false;
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

	if (util::isInRotRange(finalRot, goalRot, 2000))
	{
		cvarManager->log("Goal reached.");
	}

	this->currRecordingBuffer = 1 - this->currRecordingBuffer;
	this->getLastRecording()->analyze();
	this->isActive = false;
}

void TurningExercise::saveSnapshot()
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

Vector2 rotateVec2(Vector2F vec, float angle)
{
	int x = (int)((float)vec.X * cos(angle) - (float)vec.Y * sin(angle));
	int y = (int)((float)vec.X * sin(angle) + (float)vec.Y * cos(angle));
	return Vector2{ x, y };
}

void TurningRecording::analyze()
{
	TurningSnapshot firstSnap = snapshots.front();	
	float startAngle = -firstSnap.rotation.Yaw * M_PI / 32768 - M_PI / 2;

	int currentInput = INPUT_NONE;

	for (int i = 0; i < snapshots.size(); i++)
	{
		TurningSnapshot snap = snapshots.at(i);
		Vector relativeLoc = snap.location - firstSnap.location;
		Vector2 vec = rotateVec2(Vector2F{ relativeLoc.X, relativeLoc.Y }, startAngle);
		points.push_back(vec);

		if (vec.X < pbound.minX)
			pbound.minX = vec.X;
		if (vec.X > pbound.maxX)
			pbound.maxX = vec.X;
		if (vec.Y < pbound.minY)
			pbound.minY = vec.Y;
		if (vec.Y > pbound.maxY)
			pbound.maxY = vec.Y;

		int input = INPUT_NONE;
		if (snap.boost)
			input += INPUT_BOOST;
		if (snap.powerslide)
			input += INPUT_POWERSLIDE;
		if (snap.throttle)
			input += INPUT_THROTTLE;

		if (input != currentInput)
		{
			segments.push_back(TurningSegment{ i, input });
		}
		currentInput = input;
	}

	Vector2 firstVec = points.front();
	Vector2 lastVec = points.back();

	isTurningLeft = lastVec.X < firstVec.X;
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

LinearColor getColor(TurningSnapshot snap)
{
	if (snap.boost && snap.powerslide)
		return LinearColor{ 255, 255, 0, 255 };
	else if (snap.boost)
		return LinearColor{ 255, 0, 0, 255 };
	else if (snap.powerslide)
		return LinearColor{ 0, 255, 0, 255 };
	else
		return LinearColor{ 255, 255, 255, 255 };
}

void TurningExercise::visualize(CanvasWrapper canvas)
{
	TurningRecording* recording = this->getLastRecording();
	
	if (recording->snapshots.size() <= 0)
		return;

	canvas.SetPosition(Vector2{ 100, 100 });
	canvas.SetColor(255, 0, 0, 255);
	canvas.DrawString("# segments: " + to_string(recording->segments.size()));

	canvas.SetPosition(Vector2{ drawingX, drawingY });
	canvas.SetColor(100, 100, 100, 100);
	canvas.DrawBox(Vector2{ drawingWidth, drawingHeight });

	Vector2 origin = Vector2{ drawingX, drawingY + drawingHeight };
	if (recording->isTurningLeft)
		origin.X += drawingWidth;

	int width = recording->pbound.maxX - recording->pbound.minX;
	int height = recording->pbound.maxY - recording->pbound.minY;
	float scale = (float)max(drawingWidth,drawingHeight) / (float)max(width, height);

	Vector2 lastPoint = recording->points.front();
	Vector2 lastCoord = origin;

	for (int i = 1; i < recording->points.size(); i++)
	{
		Vector2 point = recording->points.at(i);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };

		LinearColor color = getColor(recording->snapshots.at(i));
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

		LinearColor color = getColor(recording->snapshots.at(middleIndex));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		Vector2 point = recording->points.at(middleIndex);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };
		if (recording->isTurningLeft)
			coord.X += 20;
		else
			coord.X -= 20;

		canvas.SetPosition(coord);
		canvas.DrawString(to_string(nextIndex - seg.startIndex));
	}
}