#pragma once
#include "StateInterface.hpp"
#include "DrawableVehicle.hpp"
#include "DrawableMapBuilder.hpp"
#include "DrawableVehicleBuilder.hpp"
#include "ContinuousTimer.hpp"

class DrawableDoubleText;
class ObserverInterface;

class StateMapEditor final :
	public StateInterface
{
	StateMapEditor(const StateMapEditor&) = delete;

	const StateMapEditor& operator=(const StateMapEditor&) = delete;

	StateMapEditor();

	~StateMapEditor();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;

	enum class ActiveMode;
	void SetActiveMode(ActiveMode);

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
	bool m_upToDate;
	bool m_vehiclePositioned;
	DrawableVehicle* m_drawableVehicle;
	sf::RectangleShape m_allowedMapAreaShape;
	sf::RectangleShape m_allowedViewAreaShape;

	// View movement
	ContinuousTimer m_viewMovementTimer;
	const double m_viewMovementOffset;
	double m_viewMovement;
	const double m_viewMinMovement;
	const double m_viewMaxMovement;

	// Builders
	DrawableMapBuilder m_drawableMapBuilder;
	DrawableVehicleBuilder m_drawableVehicleBuilder;

	// Texts and text observers
	enum
	{
		ACTIVE_MODE_TEXT,
		MOVEMENT_TEXT,
		VIEW_OFFSET_X_TEXT,
		VIEW_OFFSET_Y_TEXT,
		FILENAME_TEXT,
		EDGE_SUBMODE_TEXT,
		EDGE_COUNT_TEXT,
		VEHICLE_SUBMODE_TEXT,
		VEHICLE_ANGLE_TEXT,
		TEXT_COUNT
	};
	std::vector<DrawableDoubleText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;

	// Friend classes
	friend class StateManager;
};
