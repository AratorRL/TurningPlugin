#pragma once
#include "bakkesmod/wrappers/gamewrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "Recording.h"

#define INPUT_NONE 0
#define INPUT_BOOST 1
#define INPUT_POWERSLIDE 2
#define INPUT_THROTTLE 4
#define INPUT_STEER 8

class TurnExercise
{
public:
	bool isActive;

	virtual void init() = 0;

	virtual void reset() = 0;

	virtual void tick() = 0;

	virtual void end() = 0;

	virtual void clear() = 0;

	virtual void visualize(CanvasWrapper canvas);
	
	void analyzeTurn(TurningRecording* rec);

	std::shared_ptr<GameWrapper> game;
	std::shared_ptr<CVarManagerWrapper> cvarManager;


	void saveSnapshot();
	
	// 2 recording buffers: one for current recording, other for previous one
	// roles get swapped every new turn
	TurningRecording* recording[2];
	int currRecordingBuffer;
	void swapRecordingBuffers();
	TurningRecording* getCurrentRecording();
	TurningRecording* getLastRecording();
};