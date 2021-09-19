#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "DrawableVehicleBuilder.hpp"

class DrawableTextAbstract;
class ObserverIf;

class StateVehicleEditor final :
	public StateInterface
{
	enum
	{
		MODE_VEHICLE_BODY,
		MODE_VEHICLE_SENSORS
	};
	std::map<size_t, std::string> m_modeStrings;
	size_t m_mode;

	enum
	{
		INSERT,
		REMOVE
	};
	std::map<size_t, std::string> m_submodeStrings;
	size_t m_submode;

	enum
	{
		CHANGE_TO_VEHICLE_BODY_MODE,
		CHANGE_TO_VEHICLE_SENSORS_MODE,
		CHANGE_TO_INSERT_STATE,
		CHANGE_TO_REMOVE_STATE,
		CHANGE_SENSOR,
		DEACREASE_SENSOR_ANGLE,
		INCREASE_SENSOR_ANGLE,
		DECREASE_SENSOR_MOTION_RANGE,
		INCREASE_SENSOR_MOTION_RANGE,
		CONTROL_KEYS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::map<size_t, bool> m_pressedKeys;

	// Subjects of change
	sf::RectangleShape m_allowedAreaShape;
	size_t m_currentSensorIndex;
	double m_currentSensorAngle;
	double m_currentSensorMotionRange;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	bool m_upToDate;
	Line m_xAxis;
	Line m_yAxis;

	// Texts and text observers
	enum
	{
		BACK_TEXT,
		FRONT_TEXT,
		ACTIVE_MODE_TEXT,
		ACTIVE_SUBMODE_TEXT,
		TOTAL_NUMBER_OF_EDGES_TEXT,
		TOTAL_NUMBER_OF_SENSORS_TEXT,
		FILENAME_TEXT,
		CURRENT_SENSOR_TEXT,
		CURRENT_SENSOR_ANGLE_TEXT,
		CURRENT_SENSOR_MOTION_RANGE_TEXT,
		TEXT_COUNT
	};
	std::vector<DrawableTextAbstract*> m_texts;
	std::vector<ObserverIf*> m_textObservers;

public:

	StateVehicleEditor(const StateVehicleEditor&) = delete;

	const StateVehicleEditor& operator=(const StateVehicleEditor&) = delete;

	StateVehicleEditor();

	~StateVehicleEditor();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;
};
