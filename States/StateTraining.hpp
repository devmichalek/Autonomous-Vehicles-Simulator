#pragma once
#include "StateInterface.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "DrawableFilenameText.hpp"
#include "CycleTimer.hpp"

class GeneticAlgorithmNeuron;
class DrawableMap;
class DrawableCheckpointMap;
class EventObserver;

class StateTraining final :
	public StateInterface
{
	// States
	enum
	{
		STOPPED_MODE,
		RUNNING_MODE,
		MODES_COUNT
	};
	std::array<std::string, MODES_COUNT> m_modeStrings;
	size_t m_mode;

	enum
	{
		MAP_FILENAME_TYPE,
		ANN_FILENAME_TYPE,
		VEHICLE_FILENAME_TYPE,
		FILENAME_TYPES_COUNT
	};
	std::array<std::string, FILENAME_TYPES_COUNT> m_filenameTypeStrings;
	size_t m_filenameType;

	// Objects of environment
	GeneticAlgorithmNeuron* m_geneticAlgorithm;
	ArtificialNeuralNetworks m_artificialNeuralNetworks;
	DrawableMap* m_drawableMap;
	DrawableVehicleFactory m_drawableVehicleFactory;
	DrawableCheckpointMap* m_drawableCheckpointMap;

	// Builders
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;

	// Simulation parameters
	size_t m_populationSize = 30;
	size_t m_numberOfGenerations = 1000;
	size_t m_population;
	size_t m_generation;

	// Timers, offsets and value boundaries
	CycleTimer m_waveTimer;
	CycleTimer m_viewTimer;
	sf::Vector2f m_viewCenter;
	const double m_viewMovementOffset;
	const size_t m_minPopulationSize;
	const size_t m_maxPopulationSize;
	const size_t m_minNumberOfGenerations;
	const size_t m_maxNumberOfGenerations;

	// Texts
	DrawableTripleText m_modeText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableTripleText m_populationSizeText;
	DrawableTripleText m_numberOfGenerationsText;
	DrawableDoubleText m_populationText;
	DrawableDoubleText m_generationText;
	DrawableDoubleText m_highestFitnessText;
	DrawableDoubleText m_highestFitnessOverallText;
	std::vector<EventObserver*> m_eventObservers;

public:

	StateTraining(const StateTraining&) = delete;

	const StateTraining& operator=(const StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;
};
