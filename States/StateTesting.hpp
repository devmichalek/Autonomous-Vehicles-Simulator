#pragma once
#include "StateInterface.hpp"
#include "MapBuilder.hpp"
#include "VehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "ContinuousTimer.hpp"
#include "Property.hpp"
#include "SimulatedVehicle.hpp"

class DoubleText;
class ObserverInterface;
class SimulatedWorld;
class FitnessSystem;

class StateTesting final :
	public StateInterface
{
public:

	StateTesting(const StateTesting&) = delete;

	const StateTesting& operator=(const StateTesting&) = delete;

	StateTesting();

	~StateTesting();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;

private:

	// Called when new vehicle is being added
	void OnAddVehicle();

	// Called when vehicle is beign deleted
	void OnRemoveVehicle();

	// Returns current vehicle name
	std::string GetCurrentVehicleName() const;

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
		SWITCH_VEHICLE,
		NUMBER_OF_VEHICLES,
		ENABLE_USER_VEHICLE,
		ENABLE_CHECKPOINTS,
		PARAMETERS_COUNT
	};
	std::array<std::string, PARAMETERS_COUNT> m_parameterTypesStrings;
	size_t m_parameterType;

	// Control keys
	enum
	{
		CHANGE_MODE,
		CHANGE_FILENAME_TYPE,
		CHANGE_PARAMETER,
		PAUSED_CHANGE_MODE = CHANGE_PARAMETER,
		INCREASE_PARAMETER,
		DECREASE_PARAMETER,
		CONTROLS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;

	// Internal erros
	enum
	{
		ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED,
		ERROR_NO_MAP_SPECIFIED,
		ERROR_NO_VEHICLE_SPECIFIED,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH,
		ERROR_NO_VEHICLES,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Testing parameters
	size_t m_numberOfVehicles;
	size_t m_currentVehicle;
	const size_t m_maxNumberOfVehicles;
	Property<bool> m_enableUserVehicle;
	Property<bool> m_enableCheckpoints;
	Property<float> m_zoom;
	ContinuousTimer m_viewTimer;
	const double m_viewMovementOffset;
	ContinuousTimer m_pressedKeyTimer;

	// Objects of environment
	SimulatedWorld* m_simulatedWorld;
	FitnessSystem* m_fitnessSystem;
	MapPrototype* m_mapPrototype;
	SimulatedVehicle* m_userVehicle;
	VehiclePrototype* m_userVehiclePrototype;
	SimulatedVehicles m_simulatedVehicles; // Bot vehicles, pointers are cleared by world
	VehiclePrototypes m_vehiclePrototypes; // Bot vehicle prototypes
	ArtificialNeuralNetworks m_artificialNeuralNetworks; // Bot anns

	// Builders
	MapBuilder m_mapBuilder;
	VehicleBuilder m_vehicleBuilder;
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;

	// Texts and text observers
	enum
	{
		MODE_TEXT,
		FILENAME_TYPE_TEXT,
		FILENAME_TEXT,
		PARAMETER_TYPE_TEXT,
		CURRENT_VEHICLE_TEXT,
		NUMBER_OF_VEHICLES_TEXT,
		ENABLE_CHECKPOINTS_TEXT,
		ENABLE_USER_VEHICLE_TEXT,
		USER_FITNESS_TEXT,
		ZOOM_TEXT,
		TEXT_COUNT
	};
	std::vector<DoubleText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};
