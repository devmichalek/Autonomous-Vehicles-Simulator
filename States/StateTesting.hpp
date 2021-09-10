#pragma once
#include "StateInterface.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableMap.hpp"
#include "CycleTimer.hpp"

class DrawableDoubleText;
class ObserverIf;

class StateTesting final :
	public StateInterface
{
	enum
	{
		STOPPED_MODE,
		RUNNING_MODE,
		PAUSED_MODE,
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

	// Parameter types
	enum
	{
		SWITCH_VEHICLE,
		NUMBER_OF_VEHICLES,
		ACTIVATE_USER_VEHICLE,
		SHOW_CHECKPOINTS,
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
		ERROR_NO_DRAWABLE_MAP_SPECIFIED,
		ERROR_NO_DRAWABLE_VEHICLE_SPECIFIED,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH,
		ERROR_NO_DRAWABLE_VEHICLES,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Testing parameters
	size_t m_numberOfVehicles;
	size_t m_currentVehicle;
	const size_t m_maxNumberOfVehicles;
	bool m_activateUserVehicle;
	bool m_showCheckpoints;
	CycleTimer m_viewTimer;
	const double m_viewMovementOffset;

	// Objects of environment
	DrawableMap* m_drawableMap;
	DrawableVehicle* m_userVehicle;
	DrawableVehicle* m_dummyVehicle;
	DrawableVehicleFactory m_drawableVehicleFactory;
	DrawableVehicleFactory m_drawableVehicleBackup;
	ArtificialNeuralNetworks m_artificialNeuralNetworks;

	// Builders
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
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
		SHOW_CHECKPOINTS_TEXT,
		IS_USER_VEHICLE_ACTIVE_TEXT,
		USER_FITNESS_TEXT,
		TEXT_COUNT
	};
	std::vector<DrawableDoubleText*> m_texts;
	std::vector<ObserverIf*> m_textObservers;

	// Called when new vehicle is being added
	void OnAddVehicle();

	// Called when vehicle is beign deleted
	void OnRemoveVehicle();

	// Returns current vehicle name
	std::string GetCurrentVehicleName();

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
};
