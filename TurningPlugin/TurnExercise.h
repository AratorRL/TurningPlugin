#pragma once
#include "bakkesmod/wrappers/gamewrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "Recording.h"

#define INPUT_NONE 0
#define INPUT_BOOST 1
#define INPUT_POWERSLIDE 2
#define INPUT_THROTTLE 4

class TurnExercise
{
public:
	// whether the exercise is still active
	bool isActive;

	// setup valid starting state, if needed
	virtual void init() = 0;

	// reset the exercise
	virtual void reset() = 0;

	// gets called every physics tick while exercise is active
	virtual void tick() = 0;

	// end the exercise
	virtual void end() = 0;

	virtual void clear() = 0;

	virtual void visualize(CanvasWrapper canvas);
	
	void analyzeTurn(TurningRecording* rec);

	std::shared_ptr<GameWrapper> game;
	std::shared_ptr<CVarManagerWrapper> cvarManager;


	void saveSnapshot();
	TurningRecording* getCurrentRecording();
	TurningRecording* getLastRecording();
	void swapRecordingBuffers();
	
	TurningRecording* recording[2];
	int currRecordingBuffer;
	int drawingX = 300;
	int drawingY = 380;
	int drawingWidth = 200;
	int drawingHeight = 200;
};