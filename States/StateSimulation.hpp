#pragma once
#include "StateInterface.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "MapBuilder.hpp"
#include "VehicleBuilder.hpp"
#include "ContinuousTimer.hpp"
#include "Property.hpp"
#include "SimulatedVehicle.hpp"
#include "StatisticsBuilder.hpp"

class GeneticAlgorithmNeuron;
class AbstractText;
class ObserverInterface;
class SimulatedWorld;
class FitnessSystem;

class StateSimulation final :
	public StateInterface
{
public:

	StateSimulation(const StateSimulation&) = delete;

	const StateSimulation& operator=(const StateSimulation&) = delete;

	StateSimulation();

	~StateSimulation();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;

private:

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

	// File formats
	enum
	{
		MAP_FILE_FORMAT,
		ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT,
		VEHICLE_FILE_FORMAT,
		FILE_FORMATS_COUNT
	};
	std::array<std::string, FILE_FORMATS_COUNT> m_fileFormatStrings;
	size_t m_fileFormat;

	// File formats in paused mode
	enum
	{
		ARTIFICIAL_NEURAL_NETWORK_FILE_FORMAT_PAUSED,
		STATISTICS_FILE_FORMAT_PAUSED,
		FILE_FORMATS_PAUSED_COUNT
	};
	std::array<std::string, FILE_FORMATS_PAUSED_COUNT> m_fileFormatPausedStrings;
	size_t m_fileFormatPaused;

	// Parameter types
	enum
	{
		POPULATION_SIZE,
		NUMBER_OF_GENERATIONS,
		DEATH_ON_EDGE_CONTACT,
		CROSSOVER_TYPE,
		REPEAT_CROSSOVER_PER_INDIVIDUAL,
		MUTATION_PROBABILITY,
		DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS,
		NUMBER_OF_PARENTS,
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
		INCREASE_ZOOM,
		DECREASE_ZOOM,
		CONTROLS_COUNT
	};
	std::map<const size_t, const size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;

	// Internal errors
	enum
	{
		ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED,
		ERROR_NO_MAP_SPECIFIED,
		ERROR_NO_VEHICLE_SPECIFIED,
		ERROR_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Simulation properties
	Property<size_t> m_population;
	Property<size_t> m_generation;
	Property<bool> m_deathOnEdgeContact;
	Property<int> m_crossoverType;
	Property<bool> m_repeatCrossoverPerIndividual;
	Property<double> m_mutationProbability;
	Property<bool> m_decreaseMutationProbabilityOverGenerations;
	Property<size_t> m_numberOfParents;
	Property<double> m_requiredFitnessImprovement;
	Property<double> m_requiredFitnessImprovementRise;
	Property<float> m_zoom;

	// Offsets, timers
	float m_zoomThreshold;
	const double m_viewMovementOffset;
	ContinuousTimer m_viewTimer;
	ContinuousTimer m_pressedKeyTimer;
	ContinuousTimer m_requiredFitnessImprovementRiseTimer;

	// Objects of environment
	GeneticAlgorithmNeuron* m_geneticAlgorithm;
	ArtificialNeuralNetworks m_artificialNeuralNetworks;
	SimulatedWorld* m_simulatedWorld;
	FitnessSystem* m_fitnessSystem;
	SimulatedVehicles m_simulatedVehicles; // Bot vehicles, pointer are cleared by world

	// Prototypes
	ArtificialNeuralNetwork* m_artificialNeuralNetworkPrototype;
	VehiclePrototype* m_vehiclePrototype; // Bot vehicle prototype
	MapPrototype* m_mapPrototype;

	// Builders
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;
	MapBuilder m_mapBuilder;
	VehicleBuilder m_vehicleBuilder;
	StatisticsBuilder m_statisticsBuilder;

	// Texts and text observers
	enum
	{
		MODE_TEXT,
		FILE_FORMAT_STOPPED_TEXT,
		FILE_FORMAT_PAUSED_TEXT,
		FILENAME_STOPPED_TEXT,
		FILENAME_PAUSED_TEXT,
		PARAMETER_TYPE_TEXT,
		POPULATION_SIZE_TEXT,
		NUMBER_OF_GENERATIONS_TEXT,
		DEATH_ON_EDGE_CONTACT_TEXT,
		CROSSOVER_TYPE_TEXT,
		REPEAT_CROSSOVER_PER_INDIVIDUAL_TEXT,
		MUTATION_PROBABILITY_TEXT,
		DECREASE_MUTATION_PROBABILITY_OVER_GENERATIONS_TEXT,
		NUMBER_OF_PARENTS_TEXT,
		REQUIRED_FITNESS_IMPROVEMENT_RISE_TEXT,
		REQUIRED_FITNESS_IMPROVEMENT_TEXT,
		CURRENT_POPULATION_TEXT,
		CURRENT_GENERATION_TEXT,
		HIGHEST_FITNESS_TEXT,
		HIGHEST_FITNESS_OVERALL_TEXT,
		RAISING_REQUIRED_FITNESS_IMPROVEMENT_TEXT,
		MEAN_REQUIRED_FITNESS_IMPROVEMENT,
		ZOOM_TEXT,
		BEST_TIME_TEXT,
		BEST_TIME_OVERALL_TEXT,
		TEXT_COUNT
	};
	std::vector<AbstractText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};
