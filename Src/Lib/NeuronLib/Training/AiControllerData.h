#pragma once

class NeuralNetPlayerController;

class AiControllerData
{
public:
	AiControllerData();
	~AiControllerData();

public:
	NeuralNetPlayerController* m_controller = nullptr;
	int m_points = 0;
};
