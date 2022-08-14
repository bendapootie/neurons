#pragma once

class NeuralNetPlayerController;

constexpr int k_pointsForWin = 3;
constexpr int k_pointsForTie = 1;
constexpr int k_pointsForLoss = 0;



class WinLossRecord
{
public:
	void Reset()
	{
		m_wins = 0;
		m_losses = 0;
		m_ties = 0;
	}
	int GetPoints() const
	{
		return (m_wins * k_pointsForWin) + (m_ties * k_pointsForTie) + (m_losses * k_pointsForLoss);
	}
public:
	int m_wins = 0;
	int m_losses = 0;
	int m_ties = 0;
};

class AiControllerData
{
public:
	AiControllerData();
	~AiControllerData();

public:
	NeuralNetPlayerController* m_controller = nullptr;
	WinLossRecord m_winLossRecord;
};
