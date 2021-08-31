#pragma once
#include "StateInterface.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "ArtificialNeuralNetworkBuilder.hpp"
#include "DrawableMap.hpp"

class DrawableTripleText;
class ObserverIf;

class StateTesting final :
	public StateInterface
{
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

	// Control keys
	enum
	{
		CHANGE_MODE,
		CHANGE_FILENAME_TYPE,
		SWITCH_VEHICLE,
		ADD_VEHICLE,
		REMOVE_VEHICLE,
		CONTROLS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::array<bool, CONTROLS_COUNT> m_pressedKeys;

	// Internal erros
	enum
	{
		NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED,
		NO_DRAWABLE_MAP_SPECIFIED,
		NO_DRAWABLE_VEHICLE_SPECIFIED,
		ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH,
		ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH,
		INTERNAL_ERRORS_COUNT
	};
	std::array<std::string, INTERNAL_ERRORS_COUNT> m_internalErrorsStrings;

	// Testing parameters
	size_t m_numberOfVehicles;
	size_t m_currentVehicle;
	Fitness m_lastUserCalculatedFitness;
	const size_t m_maxNumberOfVehicles;

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
		ACTIVE_MODE_TEXT,
		FILENAME_TYPE_TEXT,
		FILENAME_TEXT,
		NUMBER_OF_VEHICLES_TEXT,
		CURRENT_VEHICLE_TEXT,
		USER_FITNESS,
		TEXT_COUNT
	};
	std::vector<DrawableTripleText*> m_texts;
	std::vector<ObserverIf*> m_textObservers;

	// Called when new vehicle is being added
	void OnAddVehicle();

	// Called when vehicle is beign deleted
	void OnRemoveVehicle();

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
