#include "pch.h"
#include "AiPlayerTrainer.h"

#include <algorithm>
#include "AiControllerData.h"
#include "AiControllerManager.h"
#include <fstream>
#include "NeuralNet/Network.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"
#include "Util/Random.h"
#include "Util/BinaryBuffer.h"
#include <windows.h> // for OutputDebugString


class GameStats
{
public:
	GameStats(const int index0, const int index1) :
		m_controllerIndex0(index0),
		m_controllerIndex1(index1)
	{
	}

public:
	int m_controllerIndex0 = -1;
	int m_controllerIndex1 = -1;
};


class GameSeason
{
public:
	GameSeason(int numControllers, int numSeasons)
	{
		for (int season = 0; season < numSeasons; season++)
		{
			for (int controllerIndex = 0; controllerIndex < numControllers; controllerIndex++)
			{
				const int controllerIndex0 = controllerIndex;
				const int controllerIndex1 = (controllerIndex + season + 1) % numControllers;
				m_gameStats.push_back(GameStats(controllerIndex0, controllerIndex1));
			}
		}
	}

public:
	std::vector<GameStats> m_gameStats;
};



AiPlayerTrainer::AiPlayerTrainer(const Config& config) :
	m_config(config)
{
	// Seed the random number generator from the clock
	m_rand.Seed();

	for (int i = 0; i < m_config.m_numControllers; i++)
	{
		AiControllerData* aiControllerData = new AiControllerData(m_rand);
		aiControllerData->m_controller->Randomize(m_rand);
		m_controllers.push_back(aiControllerData);
	}

	// TODO: Use multiple games once execution is multi-threaded
	m_currentGame = new NeuronGame();

	m_season = new GameSeason(config.m_numControllers, config.m_numGameSeasons);
}

AiPlayerTrainer::~AiPlayerTrainer()
{
	if (m_currentGame != nullptr)
	{
		delete m_currentGame;
	}

	for (auto controller : m_controllers)
	{
		if (controller != nullptr)
		{
			delete controller;
		}
	}
	m_controllers.clear();

	_ASSERT(m_season != nullptr);
	delete m_season;
}

void AiPlayerTrainer::Update()
{
	// Early-out if the current game index is out of range
	// This is only used to detect the end of testing condition
	// TODO: Figure out a better way to stop playing when done
	if (m_currentGameInSeason >= m_season->m_gameStats.size())
	{
		return;
	}

	NeuronGame* game = m_currentGame;

	if (game->GetGameState() == GameState::PreGame)
	{
		const GameStats& stats = m_season->m_gameStats[m_currentGameInSeason];
		
		game->SetPlayerController(0, m_controllers[stats.m_controllerIndex0]->m_controller);
		game->SetPlayerController(1, m_controllers[stats.m_controllerIndex1]->m_controller);
	}

	game->Update();

	if (game->IsGameOver())
	{
		// Record the game's score and reset the game

		const GameStats& stats = m_season->m_gameStats[m_currentGameInSeason];
		AiControllerData* p0 = m_controllers[stats.m_controllerIndex0];
		AiControllerData* p1 = m_controllers[stats.m_controllerIndex1];

		// TODO: Keep track of which AiControllerData was used for each game
		int p0Score = game->GetPlayerScore(0);
		int p1Score = game->GetPlayerScore(1);
		if (p0Score == p1Score)
		{
			p0->m_winLossRecord.m_ties++;
			p1->m_winLossRecord.m_ties++;
		}
		else if (p0Score > p1Score)
		{
			p0->m_winLossRecord.m_wins++;
			p1->m_winLossRecord.m_losses++;
		}
		else
		{
			p0->m_winLossRecord.m_losses++;
			p1->m_winLossRecord.m_wins++;
		}
		game->ResetGame(m_config.m_gameDuration);

		// Shift to next game in season
		m_currentGameInSeason++;

		// Check if all games have been run
		if (m_currentGameInSeason >= m_season->m_gameStats.size())
		{
			PrepareNextGeneration();
		}
	}
}

void AiPlayerTrainer::PrepareNextGeneration()
{
	// Sort controllers based on score.
	sort(begin(m_controllers),
		end(m_controllers),
		[](AiControllerData* a, AiControllerData* b)
		{
			int pointsCmp = (a->m_winLossRecord.GetPoints() - b->m_winLossRecord.GetPoints());
			// Divide levelsCmp by two to allow for easy growth by one level while still
			// restricting unbounded growth.
			int levelsCmp = (a->m_controller->DebugGetNetwork()->GetNumLevels() - b->m_controller->DebugGetNetwork()->GetNumLevels()) / 2;
			// More points is better. If points are equal, fewer levels is better.
			return (pointsCmp > 0) || ((pointsCmp == 0) && (levelsCmp < 0));
		});

	// Output stats
	char msg[256];
	OutputDebugStringA("========================================================================\n");

	for (int i = 0; i < m_controllers.size(); i++)
	{
		const AiControllerData* aiData = m_controllers[i];
		sprintf_s(msg, "Controller %2d = %d/%d/%d = %3d points\n",
			i,
			aiData->m_winLossRecord.m_wins,
			aiData->m_winLossRecord.m_losses,
			aiData->m_winLossRecord.m_ties,
			aiData->m_winLossRecord.GetPoints()
		);
		OutputDebugStringA(msg);
	}

	sprintf_s(msg, "%d games played in %d seasons\n", static_cast<int>(m_season->m_gameStats.size()), m_config.m_numGameSeasons);
	OutputDebugStringA(msg);
	sprintf_s(msg, "Generation %d complete =====================================\n", m_generation);
	OutputDebugStringA(msg);

	// Save controllers to disk
	if ((m_generation % m_config.m_saveEveryNGenerations == 0) ||
		(m_generation == m_config.m_numGenerations))
	{
		char filename[256];
		sprintf_s(filename, m_config.m_saveFile,
			AiControllerManager::GetFileMajorVersion(),
			AiControllerManager::GetFileMinorVersion(),
			AiControllerManager::GetFileRevisionNumber(),
			m_generation
		);

		WriteControllersToFile(filename);
	}

	// Prepare next generation
	if (m_generation != m_config.m_numGenerations)
	{
		// TODO: Keep some random other controllers too

		// Replace "dead" controllers with new ones for the next generation
		const int numControllersToKeep = static_cast<int>(m_controllers.size() * m_config.m_percentToKeep);
		for (int i = numControllersToKeep; i < m_controllers.size(); i++)
		{
			// Using only asexual reproduction for now.
			// TODO: Develop a more reliable way to get good results from merging multiple parents
			const int parentIndex = m_rand.NextInt(0, numControllersToKeep);
			const AiControllerData& parent = *m_controllers[parentIndex];
			m_controllers[i]->m_controller->Breed(m_rand, parent.m_controller);
			m_controllers[i]->m_generation = parent.m_generation + 1;
		}

		// Randomize seeding of controllers
		for (int i = 0; i < m_controllers.size() - 1; i++)
		{
			const int swapIndex = m_rand.NextInt(i + 1, static_cast<int>(m_controllers.size()));
			std::swap(m_controllers[i], m_controllers[swapIndex]);
		}

		// Reset points for next generation
		for (auto controller : m_controllers)
		{
			controller->m_winLossRecord.Reset();
		}

		m_generation++;
		m_currentGameInSeason = 0;
	}
}

void AiPlayerTrainer::WriteControllersToFile(const char* outputFileName) const
{
	if (outputFileName == nullptr)
	{
		return;
	}

	// TODO: Use a dynamically sized buffer or write each controller one at a time so
	//       not as much memory is needed, or both
	HeapBuffer buffer(1024 * 1024 * 16);	// allocate a buffer that's easily bigger than needed
	const char* magicString = AiControllerManager::GetFileMagicString();
	buffer.WriteBytes(magicString, static_cast<int>(strlen(magicString)));
	buffer.WriteInt(AiControllerManager::GetFileMajorVersion());
	buffer.WriteInt(AiControllerManager::GetFileMinorVersion());
	buffer.WriteInt(AiControllerManager::GetFileRevisionNumber());

	buffer.WriteInt(static_cast<int>(m_controllers.size()));
	for (const AiControllerData* controller : m_controllers)
	{
		controller->Serialize(buffer);
	}

	auto file = std::fstream(outputFileName, std::ios::out | std::ios::binary);
	file.write(buffer.GetPtr(), buffer.GetCurrent());
	file.close();
}


// TODO: REMOVE THIS FUNCTION!
//       It's handled by AiControllerManager now!
bool AiPlayerTrainer::ReadControllersFromFile(const char* inputFileName, int generationStride, int maxGeneration)
{
	if (inputFileName == nullptr)
	{
		return false;
	}

	// Read entire file into a vector
	// TODO: Read directly into Buffer
	std::ifstream inFile(inputFileName, std::ios::binary);
	if (inFile.fail())
	{
		// File does not exist
		int generation = maxGeneration;
		while (generation > 0)
		{
			char filename[256];
			sprintf_s(filename, m_config.m_saveFile,
				AiControllerManager::GetFileMajorVersion(),
				AiControllerManager::GetFileMinorVersion(),
				AiControllerManager::GetFileRevisionNumber(),
				generation
			);
			inFile = std::ifstream(filename, std::ios::binary);
			if (inFile.fail())
			{
				generation -= generationStride;
			}
			else
			{
				// File was found, use it and set current generation
				m_generation = generation;
				break;
			}
		}
	}
	if (inFile.fail())
	{
		return false; // "No matching file found");
	}

	std::vector<char> rawFileBytes(std::istreambuf_iterator<char>(inFile), {});
	inFile.close();

	HeapBuffer buffer(static_cast<int>(rawFileBytes.size()));
	buffer.WriteBytes(&rawFileBytes[0], static_cast<int>(rawFileBytes.size()));

	buffer.Seek(0);

	char magicString[16] = { 0 };
	int fileMajorVersion = 0;
	int fileMinorVersion = 0;
	int fileRevisionNumber = 0;
	buffer.ReadBytes(magicString, static_cast<int>(strlen(AiControllerManager::GetFileMagicString())));
	buffer.ReadInt(fileMajorVersion);
	buffer.ReadInt(fileMinorVersion);
	buffer.ReadInt(fileRevisionNumber);

	int numControllers = 0;
	buffer.ReadInt(numControllers);
	m_controllers.resize(numControllers);
	for (AiControllerData*& controller : m_controllers)
	{
		if (controller == nullptr)
		{
			controller = new AiControllerData(m_rand);
		}
		controller->Deserialize(buffer);
	}

	const bool success = buffer.GetErrorStatus() == BinaryBuffer::ErrorStatus::NoError;
	_ASSERT(success);

	return success;
}

AiControllerData* AiPlayerTrainer::GetAiController(int index)
{
	return m_controllers[index];
}

AiControllerData* AiPlayerTrainer::GetBestAiController()
{
	AiControllerData* bestController = nullptr;
	int bestPoints = 0;

	for (AiControllerData* controller : m_controllers)
	{
		const int controllerPoints = controller->m_winLossRecord.GetPoints();
		if ((bestController == nullptr) || (controllerPoints > bestPoints))
		{
			bestController = controller;
			bestPoints = controllerPoints;
		}
	}
	return bestController;
}
