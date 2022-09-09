### Toggle ImGui Menu
- [x] Kbd - Esc/F10
- [x] Controller - RB + LB + LS + RS
- [x] When ImGui Menu is up, game ignores input

## Menu
### Settings
- [x] VSync
- [x] Antialiasing
- [ ] Resolution
- [ ] Full Screen / Borderless / Windowed
### Play Game
- Player 1
	- Input 0 (kbd or controller)
	- Input 1 (kbd or controller)
	- AI
		- (+) Load from File
		- "Loaded File 0"
			- (-) Unload File
			- AI 0
			- AI 1
			- ...
		- "Loaded File 1"
		- ...
- Player 2
	- (Same as Player 1)
- Game Settings
	- Game Duration
	- Score to Win
- Start Game
### Training
- Agents
	- (Same as Player 1 : AI)
- Training Session
	- Full Game
		- Game Settings
			- (Same as Play Game : Game Settings)
	- Shot Training
		- Settings
	- Defense Training
		- Settings
- Settings
	- Generations to run (1 .. ♾)
	- Save every 'X' Generations
	- Save Filename
	- Games to show
		- 1, 4, 16, 64, 256, 1024, 4096
- Start Training