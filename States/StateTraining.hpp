#pragma once
#include "StateAbstract.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "DrawableBuilder.hpp"
#include "DrawableDoubleText.hpp"
#include "CycleTimer.hpp"
#include "StoppableTimer.hpp"

class GeneticAlgorithmNeuron;
class DrawableManager;
class DrawableCheckpointMap;

class StateTraining : public StateAbstract
{
	GeneticAlgorithmNeuron* m_evolution;
	std::vector<ArtificialNeuralNetwork> m_brains;
	DrawableBuilder m_builder;
	DrawableManager* m_manager;
	DetailedCarFactory m_carFactory;
	DrawableCheckpointMap* m_checkpointMap;

	const size_t m_populationSize = 45;
	const size_t m_numberOfGenerations = 1000;
	size_t m_generationNumber;

	CycleTimer m_waveTimer;
	CycleTimer m_viewTimer;
	
	sf::Vector2f m_viewCenter;
	const double m_viewMovementConst = 500.0;

	DrawableDoubleText m_populationText;
	DrawableDoubleText m_generationText;
	DrawableDoubleText m_highestFitnessText;
	DrawableDoubleText m_highestFitnessOverallText;

public:
	StateTraining(StateTraining&) = delete;

	StateTraining() :
		m_waveTimer(0.0, 4.0),
		m_viewTimer(1.0, 0.1)
	{
		m_evolution = nullptr;
		m_manager = nullptr;
		m_checkpointMap = nullptr;
		m_generationNumber = 1;
	}

	~StateTraining();

	void update();

	bool load();

	void draw();
};
