#pragma once
#include "StateInterface.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "DrawableFilenameText.hpp"
#include "CycleTimer.hpp"
#include "StoppableTimer.hpp"

class GeneticAlgorithmNeuron;
class DrawableMap;
class DrawableCheckpointMap;

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
	size_t m_generation;

	// Timers and offsets
	CycleTimer m_waveTimer;
	CycleTimer m_viewTimer;
	sf::Vector2f m_viewCenter;
	const double m_viewMovementOffset;

	// Texts
	DrawableTripleText m_modeText;
	DrawableFilenameText<true, false> m_filenameText;
	DrawableDoubleText m_populationText;
	DrawableDoubleText m_generationText;
	DrawableDoubleText m_highestFitnessText;
	DrawableDoubleText m_highestFitnessOverallText;
	std::vector<std::function<std::string()>> m_textFunctions;

public:
	StateTraining(const StateTraining&) = delete;

	const StateTraining& operator=(const StateTraining&) = delete;

	StateTraining();

	~StateTraining();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};
