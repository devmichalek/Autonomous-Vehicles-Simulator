#pragma once
#include "StateInterface.hpp"
#include "MapBuilder.hpp"
#include "VehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "ContinuousTimer.hpp"
#include "Property.hpp"
#include "SimulatedVehicle.hpp"

class AbstractText;
class ObserverInterface;
class SimulatedWorld;
class FitnessSystem;

class StateCompetition final :
	public StateInterface
{
public:

	StateCompetition(const StateCompetition&) = delete;

	const StateCompetition& operator=(const StateCompetition&) = delete;

	StateCompetition();

	~StateCompetition();

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

	// Returns current bot vehicle name
	std::string GetBotVehicleName() const;

	// Returns user vehicle name
	std::string GetUserVehicleName() const;

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

	// Parameter types
	enum
	{
		SWITCH_VEHICLE,
		NUMBER_OF_BOT_VEHICLES,
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
		INCREASE_ZOOM,
		DECREASE_ZOOM,
		USER_VEHICLE_TURN_LEFT,
		USER_VEHICLE_TURN_RIGHT,
		USER_VEHICLE_DRIVE_FORWARD,
		USER_VEHICLE_DRIVE_BACKWARD,
		CONTROLS_COUNT
	};
	std::map<const size_t, const size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;

	// Internal erros
	enum
	{
		ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED,
		ERROR_NO_MAP_SPECIFIED,
		ERROR_USER_VEHICLE_NOT_SPECIFIED,
		ERROR_ONE_OF_BOT_VEHICLES_IS_UNSPECIFIED,
		ERROR_USER_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN,
		ERROR_BOT_VEHICLE_IS_IN_A_COLLISION_WITH_EDGES_CHAIN,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		ERROR_USER_VEHICLE_NOT_ENABLED,
		ERROR_CANNOT_LOAD_ANN_FOR_USER_VEHICLE,
		ERROR_NO_BOT_VEHICLES,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Competition parameters
	size_t m_numberOfVehicles;
	size_t m_currentVehicle;
	const size_t m_maxNumberOfVehicles;
	Property<bool> m_enableUserVehicle;
	Property<bool> m_enableCheckpoints;
	Property<float> m_zoom;
	float m_zoomThreshold;
	ContinuousTimer m_viewTimer;
	const double m_viewMovementOffset;
	ContinuousTimer m_pressedKeyTimer;

	// Objects of environment
	SimulatedWorld* m_simulatedWorld;
	FitnessSystem* m_fitnessSystem;
	MapPrototype* m_mapPrototype;
	SimulatedVehicle* m_userVehicle;
	VehiclePrototype* m_dummyVehiclePrototype;
	VehiclePrototype* m_userVehiclePrototype;
	std::string m_userVehicleFilename;
	double m_defaultUserVehicleTorque;
	SimulatedVehicles m_simulatedVehicles; // Bot vehicles, pointers are cleared by world
	VehiclePrototypes m_vehiclePrototypes; // Bot vehicle prototypes
	ArtificialNeuralNetworks m_artificialNeuralNetworks; // Bot anns
	std::vector<std::string> m_botVehicleFilenames;
	std::vector<std::string> m_botArtificialNeuralNetworkFilenames;

	// Builders
	MapBuilder m_mapBuilder;
	VehicleBuilder m_vehicleBuilder;
	ArtificialNeuralNetworkBuilder m_artificialNeuralNetworkBuilder;

	// Texts and text observers
	enum
	{
		MODE_TEXT,
		FILE_FORMAT_TEXT,
		FILENAME_TEXT,
		PARAMETER_TYPE_TEXT,
		USER_VEHICLE_TEXT,
		BOT_VEHICLE_TEXT,
		NUMBER_OF_BOT_VEHICLES_TEXT,
		ENABLE_CHECKPOINTS_TEXT,
		ENABLE_USER_VEHICLE_TEXT,
		USER_FITNESS_TEXT,
		ZOOM_TEXT,
		TEXT_COUNT
	};
	std::vector<AbstractText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};
