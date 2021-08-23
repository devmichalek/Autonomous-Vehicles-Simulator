#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "DrawableVariableText.hpp"
#include "DrawableFilenameText.hpp"
#include "DrawableVehicleBuilder.hpp"
#include <functional>

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
		CONTROL_KEYS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::map<size_t, bool> m_pressedKeys;

	// Subjects of change
	sf::RectangleShape m_allowedAreaShape;
	double m_currentSensorAngle;
	size_t m_currentSensorIndex;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	bool m_upToDate;
	Line m_xAxis;
	Line m_yAxis;

	// Texts
	DrawableVariableText m_backText;
	DrawableVariableText m_frontText;
	DrawableTripleText m_activeModeText;
	DrawableTripleText m_activeSubmodeText;
	DrawableTripleText m_totalNumberOfEdgesText;
	DrawableTripleText m_totalNumberOfSensorsText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableTripleText m_currentSensorText;
	DrawableTripleText m_currentSensorAngleText;
	std::vector<std::function<std::string()>> m_textFunctions;

public:

	StateVehicleEditor(const StateVehicleEditor&) = delete;

	const StateVehicleEditor& operator=(const StateVehicleEditor&) = delete;

	StateVehicleEditor();

	~StateVehicleEditor();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};
