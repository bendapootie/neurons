## 8/29/2022
### Current Status
I'm stepping back a bit and rethinking things.

I've accepted there's a fundamental problem with the training approach I'm using. My fitness function is too coarse. I've been playing games like World Cup qualifying, where each generation agents play 4-8 games. This ends up giving 1,000 agents integer scores between 0 and 24. With that sort of feedback, small incremental improvements are impossible to measure and encourage from one generation to the next. The result is a tremendous amount of training for very improvements.

### Takeaways and Potential Next Steps
- Use a more incremental approach for building the AI
	- Start training in more focused scenarios that can be measured much more granularly. Examples...
		- Shooting - How close player came to hitting ball + how close ball came to scoring
		- Goalkeeping - How close player got to ball + how well it was cleared(?)
	- Create "genes" from each of the training session networks
		- Combine genes into full AI
		- Train AI in full matches using the genes to guide sexual reproduction
But first...
- Step back and build some scaffolding, starting with UI to control things instead of recompiling to edit a parameter

## 8/26/2022
### Current Status
Instead of starting initial networks with two levels, I started them with five, with 21, 13, 8, 5, and 3 neurons per level. They needed to go from 21 inputs to 3 outputs, so using Fibbonaci numbers seemed fitting. Since they have more neurons, they understandably took a bit longer to run, but I still got through 10,000 generations in one day.

After ~10,000 generations, the deeper networks were still very focused on kickoffs, but they started to show additional useful behavior if the kickoff didn't score.

### Takeaways
- I saw evidence that deeper networks were created that can produce more complex and interesting behavior.
- The fitness function I've been using is inadequate. Each agent plays against 8 random opponents and the results are scored with wins worth 3, ties 1, and losses 0. That is extremely little granularity considering there were hundreds of individual values in each of the initial networks.

### Video Status
10,000 generations with deeper networks (5-6 levels deep). Red is ranked #1, Blue is #1000.
Red misses the first kickoff but Blue's pattern after that allows it to score at least once before Red dominates the remaining kickoffs.
![](videos/2022-08-26%20Neuron%20Ball%20-%20Gen%2010000;%205-6%20Layers%20(1V1000).mp4)

This is the #1 vs #2 agents in a much less interesting match
![](videos/2022-08-26%20Neuron%20Ball%20-%20Gen%2010000;%205%20Layers%20(1V2).mp4)


## 8/25/2022
### Current Status
- Game simulations have been decoupled from rendering and can be simulated very quickly
- 1024 AI agents are created with random neural networks then played against each other
- Playing full games (60 seconds or first to 3)
- Agents with the most points (top 20%) are allowed to reproduce
- I tried both sexual and asexual reproduction
	- Two parents - Randomly chose weights and biases from each parent
	- One parent - Copied then mutated
- Ran for ~10,000 generations with each agent playing 8 games per generation
### Results
- Agents got pretty good at kickoffs, but not much else
- The best networks never organically grew beyond 2 levels
- I didn't see a tangible difference between asexual and sexual reproduction
	- I didn't have a good way to track progress, so it's possible their learning curves were different but by generation 10,000 things leveled out
	- It's more likely that the were both limited in their intelligence by the 2-level limit
### Hypotheses
- There's something discouraging networks that mutate to add a level
	- Identity levels can be added without side effects, but when they are mutated and change back to a normal activation function, the entire network could be invalidated 🤷‍♀️
- The simulation is too complex and there's not enough coverage to generate good all-purpose AI
- Sexual reproduction seems like a good idea, but it needs more structure than randomly picking neurons and weights
### Takeaways and Potential Next Steps
- Test with deeper networks
	- Fix whatever is so strongly discouraging deeper networks
	- Try starting with 3 levels
- Train with more initial starting positions than the single kickoff
- Simplify the simulation
	- Initially train on simplified elements (ie. just shooting or blocking) and combine them later
	- Remove unnecessary inputs to run more iterations
	  📝This would only save 3 inputs (my score, their score, and time remaining)
- Group neurons into genes then randomly select those from each parent rather than individual neurons
	- Try sexual reproduction with more than two parents
- Reimplement in Cuda to increase number of simulations that can be run in parallel
### Video Status
Top AI from generation 9,000 vs middle of the road AI at same generation
- Each has a  two level neural network
![](videos/2022-08-25%20Neuron%20Ball%20-%20Gen%209000;%202%20Layers%20(1V500)-1.mp4)

Top two AI from generation 9,000 playing against each other
- They are both aggressive on the kickoff, but don't react well after
![](videos/2022-08-25%20Neuron%20Ball%20-%20Gen%209000;%202%20Layers%20(1V2)-1.mp4)

## 8/08/2022
### Current Status
- The simulation, neural network, and glue between them is working well enough that two randomly seeded neural network driven AI players can play a game against each other.
- The AI's networks have 2 levels, 21 inputs, and 3 outputs. Using TanH activation function because most of the outputs want to be centered around 0 (eg. speed and steering)
- Inputs - position, velocity, facing, and boost % for each player, score and time remaining
- Outputs - Steering, speed, boost

This is one of the more interesting games
![](videos/2022-08-08%20Neuronball%20-%20First%20Ai-1.mp4)

Most of the games were less exciting and went more like this...
![](videos/2022-08-08%20Neuronball%20-%20First%20Ai-2.mp4)

## 7/06/2022
Basics of Neural Network
1. Inputs
2. Black box
3. Outputs

Graph of "neurons"