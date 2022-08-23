#pragma once

#include "NeuronBall/NeuronPlayerController.h"
#include "Util/Serializable.h"

class Network;
class Random;

class NeuralNetPlayerController : public NeuronPlayerController, ISerializable
{
public:
//	NeuralNetPlayerController();
	NeuralNetPlayerController(Random& rand);
	~NeuralNetPlayerController();

	void Serialize(BinaryBuffer& stream) const override;
	void Deserialize(BinaryBuffer& stream) override;

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) override;

	// Rewrites m_neuralNetwork by combining parents
	void Breed(const NeuralNetPlayerController& parent0, const NeuralNetPlayerController& parent1, Random& rand);
	// Rewrites m_neuralNetwork randomly
	void Randomize(Random& rand);

	const Network* DebugGetNetwork() const { return m_neuralNetwork; }

private:
	Network* m_neuralNetwork = nullptr;
};
