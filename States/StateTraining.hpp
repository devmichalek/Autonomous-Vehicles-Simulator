#pragma once
#include "StateAbstract.hpp"
#include "GeneticAlgorithm.hpp"
#include "DrawableManager.hpp"
#include "DrawableBuilder.hpp"
#include "DrawableDoubleText.hpp"
#include "CycleTimer.hpp"

class StateTraining : public StateAbstract
{
	GeneticAlgorithmNeuron* m_evolution;
	std::vector<ArtificialNeuralNetwork> m_brains;
	DrawableBuilder m_builder;
	DrawableManager* m_manager;
	DrawableCarFactory m_carFactory;
	FitnessVector m_fitnessVector;
	FitnessVector m_previousFitnessVector;
	const double m_meanFitnessConst = 0.02;

	const size_t m_populationSize = 10;
	const size_t m_numberOfGenerations = 30;
	size_t m_generationNumber;

	std::vector<double> m_carTimers;
	CycleTimer m_waveTimer;
	CycleTimer m_viewTimer;
	
	sf::Vector2f m_viewCenter;
	const double m_viewMovementConst = 500.0;

	DrawableDoubleText m_populationText;
	DrawableDoubleText m_generationText;

public:
	StateTraining(StateTraining&) = delete;

	StateTraining() :
		m_waveTimer(0.0, 4.0),
		m_viewTimer(1.0, 0.1)
	{
		m_evolution = nullptr;
		m_manager = nullptr;
		m_generationNumber = 1;
	}

	~StateTraining();

	void update();

	void load();

	void draw();
};
