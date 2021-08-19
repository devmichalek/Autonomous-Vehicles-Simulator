#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "DrawableVariableText.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
#include "DrawableVehicleBuilder.hpp"
#include <functional>

class StateVehicleEditor final : public StateInterface
{
	enum
	{
		SUBMODE_VEHICLE_BODY,
		SUBMODE_VEHICLE_SENSORS
	};
	std::map<size_t, std::string> m_submodeStrings;
	size_t m_submode;

	enum
	{
		INSERT,
		REMOVE
	};
	size_t m_submodeState;

	enum
	{
		CHANGE_TO_INSERT_STATE,
		CHANGE_TO_REMOVE_STATE,
		ESCAPE,
		FIND_NEAREST_POINT,
		DEACREASE_SENSOR_ANGLE,
		INCREASE_SENSOR_ANGLE,
		CONTROL_KEYS_COUNT
	};
	std::map<sf::Keyboard::Key, size_t> m_controlKeys;
	std::map<size_t, bool> m_pressedKeys;

	// Subjects of change
	const double m_angleOffset;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	VehicleBody m_vehicleBody;
	VehicleSensors m_vehicleSensors;
	bool m_upToDate;

	// Texts
	DrawableVariableText m_backText;
	DrawableVariableText m_frontText;
	DrawableTripleText m_totalNumberOfEdges;
	DrawableTripleText m_totalNumberOfSensors;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableDoubleText m_currentSensorName;
	DrawableTripleText m_currentSensorAngle;
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
