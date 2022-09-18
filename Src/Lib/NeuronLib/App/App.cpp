#include "pch.h"
#include "App.h"

#include <chrono>
#include "imgui/imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include "NeuronBall/Controllers/HumanPlayerController.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"
#include <SFML/Graphics.hpp>
#include "Training/AiControllerData.h"
#include "Training/AiPlayerTrainer.h"
#include "Util/WindowsDialogs.h"

enum class PlayMode
{
	TrainAiControllers,
	VsSavedAi,
	PlayerVsPlayer
};

// Default settings
// Duration = 60 seconds
// Controllers = 1024
// GamesPerSeason = 4
// Keep 20% per generation

constexpr PlayMode k_playMode = PlayMode::PlayerVsPlayer;
constexpr float k_gameDuration = 15.0f * 1.0f;

constexpr int k_numControllers = 1024;
constexpr int m_numGameSeasons = 8;
constexpr float k_percentControllersToKeepPerGeneration = 0.2f;
constexpr int k_saveEveryNGenerations = 100;
constexpr int k_numGenerationsToRun = 1000 * 1000;
const char* k_saveFileName = "Ai_v%d_%d_%d_deep_%dgen.bin";
//const char* k_loadFileName = "Ai_v0_1_0_deep_14400gen.bin";
const char* k_loadFileName = "Ai_v0_1_0_deep_10600gen.bin";

// Disable vsync when training AI so the simulations can run as fast as possible
constexpr bool k_vsyncEnabled = (k_playMode != PlayMode::TrainAiControllers);
constexpr float k_windowWidth = 1280;	// 1280
constexpr float k_aspectRatio = 16.0f / 9.0f;

constexpr int k_startupDelay = 60 * 0;

constexpr bool k_imguiEnabled = true;

// TODO: Move these into somewhere nicer, probably a struct in App
static bool s_vsyncEnabled = (k_playMode != PlayMode::TrainAiControllers);
static int s_antialiasingValue = 0;
// TODO: Make a nicer way to set antialiasing values, or at least hide it better
const std::vector<int> k_antialiasingValueToLevel = { 0, 2, 4, 8, 16 };

const int k_numSupportedJoysticks = 4;

App::~App()
{
	if (m_aiPlayerTrainer != nullptr)
	{
		delete m_aiPlayerTrainer;
	}

	if (m_testGame != nullptr)
	{
		delete m_testGame;
	}
}

void App::Initialize()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = k_antialiasingValueToLevel[s_antialiasingValue];
	m_window.create(
		sf::VideoMode(static_cast<int>(k_windowWidth), static_cast<int>(Math::Ceil(k_windowWidth / k_aspectRatio))),
		"Neurons",
		sf::Style::Default,
		settings
	);
	m_window.setVerticalSyncEnabled(s_vsyncEnabled);

	if constexpr(k_imguiEnabled == true)
	{
		ImGui::SFML::Init(m_window);

		ImGuiIO& io = ImGui::GetIO();
		// Set to "true" to hide the OS's cursor and have ImGui draw its own
		io.MouseDrawCursor = false;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	}

	InitializeGame();
}

int App::Run()
{
	int returnCode = 0;
	while (m_window.isOpen())
	{
		// Only update the display if vsync is on or enough time has passed
		m_renderThisFrame = s_vsyncEnabled;
		if (!s_vsyncEnabled)
		{
			auto now = std::chrono::high_resolution_clock::now();
			static auto s_lastDisplayTime = now;
			auto timeSinceUpdate = now - s_lastDisplayTime;
			auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceUpdate);
			if (elapsedMilliseconds.count() >= 16)
			{
				m_renderThisFrame = true;
				s_lastDisplayTime = now;
			}
		}

		sf::Event event;
		// Only poll for OS events if the display was updated. It wastes CPU to do it more often.
		if (m_renderThisFrame)
		{
			while (m_window.pollEvent(event))
			{
				if constexpr(k_imguiEnabled)
				{
					ImGui::SFML::ProcessEvent(m_window, event);
				}

				if (event.type == sf::Event::Closed)
				{
					m_window.close();
					if constexpr(k_imguiEnabled == true)
					{
						ImGui::SFML::Shutdown();
					}
				}
			}
		}

		if (m_window.isOpen())
		{
			if (m_renderThisFrame)
			{
				if constexpr (k_imguiEnabled == true)
				{
					// TODO: Pass in more accurate deltaTime value instead of assuming it's 60 fps
					ImGui::SFML::Update(m_window, sf::seconds(static_cast<float>(1.0 / 60.0f)));
				}
			}

			static int s_startupDelay = k_startupDelay;
			if (s_startupDelay > 0)
			{
				s_startupDelay--;
			}
			else
			{
				UpdateGame();
			}

			if (m_renderThisFrame)
			{
				DrawGame();
			}
		}
	}
	return returnCode;
}

void App::InitializeGame()
{
	switch (k_playMode)
	{
	case PlayMode::TrainAiControllers:
	{
		AiPlayerTrainer::Config config;
		config.m_numControllers = k_numControllers;
		config.m_numGameSeasons = m_numGameSeasons;
		config.m_gameDuration = k_gameDuration;
		config.m_percentToKeep = k_percentControllersToKeepPerGeneration;
		config.m_saveEveryNGenerations = k_saveEveryNGenerations;
		config.m_numGenerations = k_numGenerationsToRun;
		config.m_saveFile = k_saveFileName;

		_ASSERT(m_aiPlayerTrainer == nullptr);
		m_aiPlayerTrainer = new AiPlayerTrainer(config);

		bool success = m_aiPlayerTrainer->ReadControllersFromFile(k_saveFileName, k_saveEveryNGenerations, k_numGenerationsToRun);
		// TODO: Maybe output a message about whether or not data was read from a file

		break;
	}

	case PlayMode::VsSavedAi:
	{
		// TODO: Figure out a better way of loading AI controllers without instantiating m_aiPlayerTrainer
		AiPlayerTrainer::Config dummyConfig;
		m_aiPlayerTrainer = new AiPlayerTrainer(dummyConfig);
		bool success = m_aiPlayerTrainer->ReadControllersFromFile(k_loadFileName);
		// Fail loudly if saved controllers couldn't be read
		_ASSERT(success);

		_ASSERT(m_testGame == nullptr);
		m_testGame = new NeuronGame();
		m_testGame->SetPlayerController(0, new HumanPlayerController(InputProvider(0, m_userInputBlocker)));
		m_testGame->SetPlayerController(1, m_aiPlayerTrainer->GetAiController(0)->m_controller);
//		m_testGame->SetPlayerController(1, m_aiPlayerTrainer->GetAiController(1000)->m_controller);
		break;
	}
	break;

	case PlayMode::PlayerVsPlayer:
	{
		_ASSERT(m_testGame == nullptr);
		m_testGame = new NeuronGame();
		m_testGame->SetPlayerController(0, new HumanPlayerController(InputProvider(0, m_userInputBlocker)));
		m_testGame->SetPlayerController(1, new HumanPlayerController(InputProvider(1, m_userInputBlocker)));
		break;
	}

	default:
		break;
	}
}

void App::UpdateGame()
{
	if (k_playMode == PlayMode::TrainAiControllers)
	{
		m_aiPlayerTrainer->Update();
	}
	else
	{
		m_testGame->Update();
	}
}

void App::DrawGame()
{
	_ASSERT(m_renderThisFrame);

	const NeuronGame* gameToDisplay = m_testGame;
	if (k_playMode == PlayMode::TrainAiControllers)
	{
		gameToDisplay = m_aiPlayerTrainer->GetGame(0);
	}

	const float length = gameToDisplay->GetFieldLength();
	const float width = gameToDisplay->GetFieldWidth();
	const float viewSize = length * 2.0f;
	sf::View view(sf::Vector2f(length * 0.5f, width * 0.5f), sf::Vector2f(viewSize, viewSize / k_aspectRatio));
	m_window.setView(view);
	m_window.clear(sf::Color(0, 0, 32, 255));

	NeuronGameDisplay gameDisplay(*gameToDisplay);
	gameDisplay.Draw(m_window);

	UpdateDebugMenu();

	if constexpr (k_imguiEnabled)
	{
		ImGui::SFML::Render(m_window);
	}
	m_window.display();
}

void App::UpdateDebugMenu()
{
	if (k_imguiEnabled == false)
	{
		return;
	}

	CheckForDebugMenuToggle();

	if (m_menuOpen == false)
	{
		return;
	}

	// TODO: Add a debug key or controller button combination to open the menu
	//       When the menu is open, all input should be handled by ImGui instead of the game

	if (ImGui::BeginMainMenuBar())
	{
		DebugMenuHelper_UpdateDisplay();
		DebugMenuHelper_UpdatePlayGame();
		DebugMenuHelper_UpdateTraining();

		ImGui::EndMainMenuBar();
	}

	// Debug display of contoller input
	static bool s_joystickDisplayActive = false;
	if (s_joystickDisplayActive)
	{
		ImGui::Begin("Controllers", &s_joystickDisplayActive);
		ImGui::BeginTabBar("Controllers");
		for (int joystickIndex = 0; joystickIndex < k_numSupportedJoysticks; joystickIndex++)
		{
			if (sf::Joystick::isConnected(joystickIndex))
			{
				sf::Joystick::Identification id = sf::Joystick::getIdentification(joystickIndex);
				std::string joystickName = id.name.toAnsiString();

				if (ImGui::BeginTabItem(joystickName.c_str()))
				{
					// Show button states
					const int numButtons = sf::Joystick::getButtonCount(joystickIndex);
					for (int buttonIndex = 0; buttonIndex < numButtons; buttonIndex++)
					{
						const bool pressed = sf::Joystick::isButtonPressed(joystickIndex, buttonIndex);
						const ImVec4 pressedColor(1.0f, 1.0f, 1.0f, 1.0f);
						const ImVec4 releasedColor(1.0f, 1.0f, 1.0f, 0.5f);
						ImGui::TextColored(pressed ? pressedColor : releasedColor, "Button %d%s", buttonIndex, pressed ? " Pressed" : "");
					}

					// Show axes state
					const int k_maxAxisIndex = static_cast<int>(sf::Joystick::Axis::PovY) + 1;
					for (int axisIndex = 0; axisIndex < k_maxAxisIndex; axisIndex++)
					{
						sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(axisIndex);
						if (sf::Joystick::hasAxis(joystickIndex, axis))
						{
							char msg[64];
							sprintf_s(msg, "Axis %d", axisIndex);
							const float position = sf::Joystick::getAxisPosition(joystickIndex, axis);
							const float percent = (position + 100.0f) / 200.0f;
							ImGui::ProgressBar(percent, ImVec2(-1.0f, 0.f), msg);
						}
					}

					ImGui::EndTabItem();
				}
			}
		}
		ImGui::EndTabBar();
		ImGui::End();
	}

	ImGui::ShowDemoWindow();
}

void App::DebugMenuHelper_UpdateDisplay()
{
	if (ImGui::BeginMenu("Display"))
	{
		if (ImGui::Checkbox("VSync", &s_vsyncEnabled))
		{
			m_window.setVerticalSyncEnabled(s_vsyncEnabled);
		}

		if (ImGui::SliderInt("Antialiasing", &s_antialiasingValue, 0, static_cast<int>(k_antialiasingValueToLevel.size() - 1)))
		{
			_ASSERT((s_antialiasingValue >= 0) && (s_antialiasingValue < k_antialiasingValueToLevel.size()));
		}

		// Only show "Apply Changes" option if the Antialiasing setting has changed
		int newAntialiasingLevel = k_antialiasingValueToLevel[s_antialiasingValue];
		if (m_window.getSettings().antialiasingLevel != newAntialiasingLevel)
		{
			if (ImGui::Button("Apply Changes"))
			{
				// Antialiasing can only be changed by recreating the render window
				// TODO: Consider using render to texture so the entire window doesn't need to be recreated

				// TODO: Window creation code here is identical to initialization. It should be merged.
				sf::ContextSettings settings;
				settings.antialiasingLevel = newAntialiasingLevel;
				m_window.create(
					sf::VideoMode(static_cast<int>(k_windowWidth), static_cast<int>(Math::Ceil(k_windowWidth / k_aspectRatio))),
					"Neurons",
					sf::Style::Default,
					settings
				);
				m_window.setVerticalSyncEnabled(s_vsyncEnabled);

				// Make sure the new setting is actually what we wanted.
				// This may throw false positives if run on a device that doesn't support
				// the expected AA levels I hard-coded in
				_ASSERT(m_window.getSettings().antialiasingLevel == newAntialiasingLevel);
			}
		}

		ImGui::EndMenu();
	}
}

void App::DebugMenuHelper_UpdatePlayGame()
{
	if (ImGui::BeginMenu("Play"))
	{
		for (int playerIndex = 0; playerIndex < k_numPlayers; playerIndex++)
		{
			char str[64];
			sprintf_s(str, "Player %d", playerIndex);
			if (ImGui::BeginMenu(str))
			{
				// TODO: Track each player's selection separately
				static int s_tempRadioValue = 0;
				ImGui::RadioButton("Input 0", &s_tempRadioValue, 0);
				ImGui::RadioButton("Input 1", &s_tempRadioValue, 1);
				ImGui::RadioButton("AI", &s_tempRadioValue, 2);
				ImGui::EndMenu();
			}
		}

		if (ImGui::BeginMenu("Game Settings"))
		{
			// TODO: Don't use static variables for this!
			static int s_gameDuration = 60;
			ImGui::SliderInt("Game Duration", &s_gameDuration, 0, 300);

			// TODO: Don't use static variables for this!
			static int s_scoreToWin = 5;
			ImGui::SliderInt("Score to Win slider", &s_scoreToWin, 1, 100);

			ImGui::EndMenu();
		}

		if (ImGui::Button("Start Game"))
		{
		}

		ImGui::EndMenu();
	}
}

void App::DebugMenuHelper_UpdateTraining()
{
	if (ImGui::BeginMenu("Training"))
	{
		if (ImGui::BeginMenu("Agents"))
		{
			if (ImGui::Button("Load from File"))
			{
				OpenFileDialog openFileDialog;
				const bool success = openFileDialog.Show();
				if (success)
				{
					std::string filePath = openFileDialog.GetFullPath();
					m_controllerMap.LoadFromFile(filePath);
				}
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("File: poop.bin"))
			{
				ImGui::Text("Agent 0; 6 neurons, score = 0.28, generation = 6");
				ImGui::Text("Agent 1; 97 neurons, score = 0.27, generation = 6");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("File: fart.bin"))
			{
				ImGui::Text("Agent 0; 53 neurons, score = 3.0, generation = 640");
				ImGui::Text("Agent 1; 55 neurons, score = 2.87, generation = 640");
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Scenario"))
		{
			if (ImGui::BeginMenu("Full Game"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Shoot Training"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Defending"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			static int s_numGenerations = 100;
			ImGui::InputInt("Generations to Run", &s_numGenerations);
			static float s_percentToKeep = 20.0f;
			ImGui::SliderFloat("Percent to Keep", &s_percentToKeep, 0.0f, 100.0f, "%.0f%%", 1.0f);
			static float s_mutationRate = 0.1f;
			ImGui::SliderFloat("Mutation Rate", &s_mutationRate, 0.0f, 1.0f, "%0.2f", 1.0f);
			static int s_gamesToDisplay = 1;
			ImGui::SliderInt("Games to Display", &s_gamesToDisplay, 0, 4096, "%d");
			ImGui::Button("Start Training");

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void App::CheckForDebugMenuToggle()
{
	bool toggleMenu = false;

	// Check for keyboard shortcuts (Esc and F10)
	const bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
	if (escPressed && !m_uiMenuToggleEscDownLastFrame)
	{
		toggleMenu = true;
	}
	m_uiMenuToggleEscDownLastFrame = escPressed;

	const bool f10Pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F10);
	if (f10Pressed && !m_uiMenuToggleF10DownLastFrame)
	{
		toggleMenu = true;
	}
	m_uiMenuToggleF10DownLastFrame = f10Pressed;

	// Check for controller shortcuts (RB + LB + LS + RS)
	bool allClawButtonsPressed = false;
	for (int joystickIndex = 0; joystickIndex < k_numSupportedJoysticks; joystickIndex++)
	{
		if (sf::Joystick::isConnected(joystickIndex))
		{
			bool allButtonsPressed = true;
			const std::vector<int> k_requiredButtons = { 4, 5, 8, 9 };
			for (int buttonIndex : k_requiredButtons)
			{
				allButtonsPressed &= sf::Joystick::isButtonPressed(joystickIndex, buttonIndex);
			}
			// TODO: Handle each controller's input independently 
			allClawButtonsPressed |= allButtonsPressed;
		}
	}
	if (allClawButtonsPressed && !m_uiMenuToggleClawDownLastFrame)
	{
		toggleMenu = true;
	}
	m_uiMenuToggleClawDownLastFrame = allClawButtonsPressed;

	// If 'toggleMenu' was set, toggle the menu
	if (toggleMenu)
	{
		if (m_menuOpen == false)
		{
			m_menuOpen = true;
			m_userInputBlocker.AddRef();
		}
		else
		{
			m_menuOpen = false;
			m_userInputBlocker.Release();
		}
	}
}
