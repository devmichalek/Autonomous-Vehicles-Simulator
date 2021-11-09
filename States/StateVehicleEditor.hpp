#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "VehicleBuilder.hpp"

class AbstractText;
class ObserverInterface;

class StateVehicleEditor final :
	public StateInterface
{
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

	// Inserts sensor based on given position
	void InsertSensor(const sf::Vector2f& point);

	// Remove sensor based on given position
	void RemoveSensor(const sf::Vector2f& point);

private:

	// Modes
	enum
	{
		MODE_VEHICLE_BODY,
		MODE_VEHICLE_SENSORS,
		MODES_COUNT
	};
	std::array<std::string, MODES_COUNT> m_modeStrings;
	size_t m_mode;

	// Vehicle sensors submodes
	enum
	{
		VEHICLE_SENSORS_INSERT,
		VEHICLE_SENSORS_REMOVE,
		VEHICLE_SENSORS_SUBMODE_COUNT
	};
	std::array<std::string, VEHICLE_SENSORS_SUBMODE_COUNT> m_vehicleSensorsSubmodeStrings;
	size_t m_vehicleSensorsSubmode;

	// Control keys
	enum
	{
		CHANGE_TO_VEHICLE_BODY_MODE,
		CHANGE_TO_VEHICLE_SENSORS_MODE,
		CHANGE_TO_INSERT_STATE,
		CHANGE_TO_REMOVE_STATE,
		REMOVE_LAST_VEHICLE_BODY_POINT,
		CHANGE_SENSOR,
		DEACREASE_SENSOR_ANGLE,
		INCREASE_SENSOR_ANGLE,
		DECREASE_SENSOR_MOTION_RANGE,
		INCREASE_SENSOR_MOTION_RANGE,
		CONTROL_KEYS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::array<bool, CONTROL_KEYS_COUNT> m_pressedKeys;

	// Subjects of change
	sf::RectangleShape m_allowedAreaShape;
	size_t m_currentSensorIndex;
	double m_currentSensorAngle;
	double m_currentSensorMotionRange;
	VehicleBuilder m_vehicleBuilder;
	VehiclePrototype* m_vehiclePrototype;
	bool m_upToDate;
	EdgeShape m_xAxis;
	EdgeShape m_yAxis;

	// Texts and text observers
	enum
	{
		BACK_TEXT,
		FRONT_TEXT,
		MODE_TEXT,
		TOTAL_NUMBER_OF_BODY_POINTS_TEXT,
		TOTAL_NUMBER_OF_SENSORS_TEXT,
		FILENAME_TEXT,
		VEHICLE_BODY_MASS,
		VEHICLE_SENSORS_SUBMODE_TEXT,
		CURRENT_SENSOR_TEXT,
		CURRENT_SENSOR_ANGLE_TEXT,
		CURRENT_SENSOR_MOTION_RANGE_TEXT,
		TEXT_COUNT
	};
	std::vector<AbstractText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};
