#pragma once
#include "StateInterface.hpp"
#include "DrawableVehicle.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "CycleTimer.hpp"
#include <functional>

class StateMapEditor final : public StateInterface
{
	enum class ActiveMode
	{
		EDGE,
		VEHICLE
	} m_activeMode;
	std::map<ActiveMode, std::string> m_activeModeMap;

	enum class EdgeSubmode
	{
		GLUED_INSERT,
		REMOVE
	} m_edgeSubmode;
	std::map<EdgeSubmode, std::string> m_edgeSubmodeMap;

	enum class VehicleSubmode
	{
		INSERT,
		REMOVE
	} m_vehicleSubmode;
	std::map<VehicleSubmode, std::string> m_vehicleSubmodeMap;

	Line m_line;
	EdgeVector m_edges;
	bool m_insertEdge;
	bool m_removeEdge;
	sf::Vector2f m_edgeBeggining;

	CycleTimer m_movementTimer;
	bool m_upToDate;

	bool m_vehiclePositioned;
	DrawableVehicle* m_drawableVehicle;

	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;
	bool m_spaceKeyPressed;

	DrawableTripleText m_activeModeText;
	DrawableTripleText m_movementText;
	DrawableTripleText m_viewOffsetXText;
	DrawableTripleText m_viewOffsetYText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableTripleText m_edgeSubmodeText;
	DrawableDoubleText m_edgeCountText;
	DrawableTripleText m_vehicleSubmodeText;
	DrawableTripleText m_vehicleAngleText;
	std::vector<std::function<std::string()>> m_textFunctions;

	void SetActiveMode(ActiveMode);

public:

	StateMapEditor(const StateMapEditor&) = delete;

	const StateMapEditor& operator=(const StateMapEditor&) = delete;

	StateMapEditor();

	~StateMapEditor();

	void Reload();

	void Capture();

	void Update();

	bool Load();

	void Draw();
};
