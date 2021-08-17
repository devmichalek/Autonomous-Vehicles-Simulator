#pragma once
#include "StateInterface.hpp"
#include "DrawableMath.hpp"
#include "DrawableVariableText.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
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

	EdgeVector m_edges;
	std::vector<sf::Vector2f> m_sensorPositions;
	std::vector<double> m_sensorAngles;

	// Texts
	DrawableVariableText m_backText;
	DrawableVariableText m_frontText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableDoubleText m_totalNumberOfEdges;
	DrawableDoubleText m_totalNumberOfSensors;
	DrawableDoubleText m_currentSensorAngle;
	DrawableTripleText m_currentSensorName;
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
