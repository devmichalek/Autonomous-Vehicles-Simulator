#pragma once
#include "StateAbstract.hpp"
#include "GeneticAlgorithm.hpp"

class StateTraining : public StateAbstract
{
	GeneticAlgorithmNeuron* m_evolution;
	std::vector<ArtificialNeuralNetwork> m_brains;

public:
	StateTraining(StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void update();

	void load();

	void draw();
};
