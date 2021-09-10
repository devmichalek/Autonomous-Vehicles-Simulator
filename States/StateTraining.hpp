#pragma once
#include "StateInterface.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "CycleTimer.hpp"

class GeneticAlgorithmNeuron;
class DrawableMap;
class DrawableCheckpointMap;
class DrawableDoubleText;
class ObserverIf;

class StateTraining final :
	public StateInterface
{
	// Modes
	enum
	{
		STOPPED_MODE,
		RUNNING_MODE,
		PAUSED_MODE,
		MODES_COUNT
	};
	std::array<std::string, MODES_COUNT> m_modeStrings;
	size_t m_mode;

	// Filename types
	enum
	{
		MAP_FILENAME_TYPE,
		ANN_FILENAME_TYPE,
		VEHICLE_FILENAME_TYPE,
		FILENAME_TYPES_COUNT
	};
	std::array<std::string, FILENAME_TYPES_COUNT> m_filenameTypeStrings;
	size_t m_filenameType;

	// Parameter types
	enum
	{
		POPULATION_SIZE,
		NUMBER_OF_GENERATIONS,
		CROSSOVER_PROBABILITY,
		MUTATION_PROBABILITY,
		DECREASE_MUTATION_OVER_GENERATIONS,
		SINGLE_POINT_CROSSOVER,
		REQUIRED_FITNESS_IMPROVEMENT_RISE,
		REQUIRED_FITNESS_IMPROVEMENT,
		PARAMETERS_COUNT
	};
	std::array<std::string, PARAMETERS_COUNT> m_parameterTypesStrings;
	size_t m_parameterType;

	// Control keys
	enum
	{
		CHANGE_MODE,
		CHANGE_FILENAME_TYPE,
		CHANGE_SIMULATION_PARAMETER,
		PAUSED_CHANGE_MODE = CHANGE_SIMULATION_PARAMETER,
		INCREASE_PARAMETER,
		DECREASE_PARAMETER,
		CONTROLS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;

	// Internal errors
	enum
	{
		ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED,
		ERROR_NO_DRAWABLE_MAP_SPECIFIED,
		ERROR_NO_DRAWABLE_VEHICLE_SPECIFIED,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH,
		ERROR_SAVE_IS_ALLOWED_ONLY_IN_PAUSED_MODE,
		ERROR_SAVE_IS_ALLOWED_ONLY_FOR_ANN,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Simulation parameters
	size_t m_populationSize;
	size_t m_numberOfGenerations;
	size_t m_population;
	size_t m_generation;
	double m_crossoverProbability;
	double m_mutationProbability;
	bool m_decreaseMutationOverGenerations;
	bool m_singlePointCrossover;
	double m_requiredFitnessImprovement;
	double m_meanRequiredFitnessImprovement;

	// Objects of environment
	GeneticAlgorithmNeuron* m_geneticAlgorithm;
	ArtificialNeuralNetworks m_artificialNeuralNetworks;
	DrawableMap* m_drawableMap;
	DrawableVehicleFactory m_drawableVehicleFactory;

	// Backups
	ArtificialNeuralNetwork* m_artificialNeuralNetworkBackup;
	DrawableVehicle* m_drawableVehicleBackup;
	DrawableMap* m_drawableMapBackup;

	// Builders
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;

	// Timers, offsets and value boundaries
	CycleTimer m_pressedKeyTimer;
	CycleTimer m_viewTimer;
	CycleTimer m_requiredFitnessImprovementRiseTimer;
	const double m_viewMovementOffset;
	const size_t m_minPopulationSize;
	const size_t m_maxPopulationSize;
	const size_t m_populationSizeResetValue;
	const size_t m_minNumberOfGenerations;
	const size_t m_maxNumberOfGenerations;
	const size_t m_numberOfGenerationsResetValue;
	const double m_minCrossoverProbability;
	const double m_maxCrossoverProbability;
	const double m_crossoverProbabilityResetValue;
	const double m_crossoverProbabilityOffset;
	const double m_minMutationProbability;
	const double m_maxMutationProbability;
	const double m_mutationProbabilityResetValue;
	const double m_mutationProbabilityOffset;
	const double m_minRequiredFitnessImprovement;
	const double m_maxRequiredFitnessImprovement;
	const double m_requiredFitnessImprovementResetValue;
	const double m_requiredFitnessImprovementOffset;
	const double m_minRequiredFitnessImprovementRise;
	const double m_maxRequiredFitnessImprovementRise;
	const double m_requiredFitnessImprovementRiseResetValue;
	const double m_requiredFitnessImprovementRiseOffset;

	// Texts and text observers
	enum
	{
		MODE_TEXT,
		FILENAME_TYPE_TEXT,
		FILENAME_TEXT,
		PARAMETER_TYPE_TEXT,
		POPULATION_SIZE_TEXT,
		NUMBER_OF_GENERATIONS_TEXT,
		CROSSOVER_PROBABILITY_TEXT,
		MUTATION_PROBABILITY_TEXT,
		DECREASE_MUTATION_OVER_GENERATION_TEXT,
		SINGLE_POINT_CROSSOVER_TEXT,
		REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT,
		REQUIRED_FITNESS_IMPROVEMENT_TEXT,
		CURRENT_POPULATION_TEXT,
		CURRENT_GENERATION_TEXT,
		HIGHEST_FITNESS_TEXT,
		HIGHEST_FITNESS_OVERALL_TEXT,
		RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT,
		MEAN_REQUIRED_FITNESS_IMPROVEMENT,
		TEXT_COUNT
	};
	std::vector<DrawableDoubleText*> m_texts;
	std::vector<ObserverIf*> m_textObservers;

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
