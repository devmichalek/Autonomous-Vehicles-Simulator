#pragma once
#include "StateInterface.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "DrawableDoubleText.hpp"
#include "CycleTimer.hpp"
#include "StoppableTimer.hpp"

class GeneticAlgorithmNeuron;
class DrawableEdgeManager;
class DrawableCheckpointMap;

class StateTraining final : public StateInterface
{
	GeneticAlgorithmNeuron* m_evolution;
	std::vector<ArtificialNeuralNetwork> m_brains;
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	DrawableEdgeManager* m_edgeManager;
	DetailedVehicleFactory m_vehicleFactory;
	DrawableCheckpointMap* m_checkpointMap;

	const size_t m_populationSize = 30;
	const size_t m_numberOfGenerations = 1000;
	const size_t m_annNumberOfInputs = 5;
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
	StateTraining(const StateTraining&) = delete;

	const StateTraining& operator=(const StateTraining&) = delete;

	StateTraining() :
		m_waveTimer(0.0, 4.0),
		m_viewTimer(1.0, 0.1)
	{
		m_evolution = nullptr;
		m_edgeManager = nullptr;
		m_checkpointMap = nullptr;
		m_generationNumber = 1;
	}

	~StateTraining();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};
