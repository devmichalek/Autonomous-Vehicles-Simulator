#pragma once
#include "StateInterface.hpp"
#include "MapBuilder.hpp"
#include "VehicleBuilder.hpp"
#include "ContinuousTimer.hpp"
#include "Property.hpp"

class AbstractText;
class ObserverInterface;

class StateMapEditor final :
	public StateInterface
{
public:

	StateMapEditor(const StateMapEditor&) = delete;

	const StateMapEditor& operator=(const StateMapEditor&) = delete;

	StateMapEditor();

	~StateMapEditor();

	void Reload() override;

	void Capture() override;

	void Update() override;

	bool Load() override;

	void Draw() override;

private:

	// Called on edge insertion
	void InsertEdge(sf::Vector2f edgeEndPoint);

	// Called on edge removal
	void RemoveEdge(sf::Vector2f edgeEndPoint);

	// Modes
	enum
	{
		EDGE_MODE,
		VEHICLE_MODE,
		MODES_COUNT
	};
	std::array<std::string, MODES_COUNT> m_modeStrings;
	size_t m_mode;

	// Edge submodes
	enum
	{
		GLUED_INSERT_EDGE_SUBMODE,
		REMOVE_EDGE_SUBMODE,
		EDGE_SUBMODE_COUNT
	};
	std::array<std::string, EDGE_SUBMODE_COUNT> m_edgeSubmodeStrings;
	size_t m_edgeSubmode;

	enum
	{
		CHANGE_TO_EDGE_MODE,
		CHANGE_TO_VEHICLE_MODE,
		INCREASE_MOVEMENT,
		DECREASE_MOVEMENT,
		INCREASE_ZOOM,
		DECREASE_ZOOM,
		MOVE_OFFSET_LEFT,
		MOVE_OFFSET_RIGHT,
		MOVE_OFFSET_UP,
		MOVE_OFFSET_DOWN,
		CHANGE_TO_EDGE_MODE_INSERT_STATE,
		CHANGE_TO_EDGE_MODE_REMOVE_STATE,
		FIND_NEAREST_EDGE_POINT,
		CANCEL_EDGE,
		INCREASE_VEHICLE_ANGLE,
		DECREASE_VEHICLE_ANGLE,
		REMOVE_VEHICLE,
		CONTROL_KEYS_COUNT
	};
	std::map<const size_t, const size_t> m_controlKeys;
	std::array<bool, CONTROL_KEYS_COUNT> m_pressedKeys;
	ContinuousTimer m_pressedKeyTimer;

	VehiclePrototype* m_vehiclePrototype;
	MapPrototype m_mapPrototype;
	bool m_insertEdge;
	bool m_removeEdge;
	sf::Vector2f m_edgeBeggining;
	bool m_upToDate;
	bool m_vehiclePositioned;
	sf::RectangleShape m_allowedMapAreaShape;
	sf::RectangleShape m_allowedViewAreaShape;
	Property<double> m_viewMovement;
	Property<float> m_zoom;

	// Builders
	MapBuilder m_mapBuilder;
	VehicleBuilder m_vehicleBuilder;

	// Texts and text observers
	enum
	{
		MODE_TEXT,
		MOVEMENT_TEXT,
		ZOOM_TEXT,
		VIEW_OFFSET_TEXT,
		FILENAME_TEXT,
		EDGE_SUBMODE_TEXT,
		NUMBER_OF_INNER_EDGES_TEXT,
		NUMBER_OF_OUTER_EDGES_TEXT,
		INNER_EDGES_CHAIN_COMPLETED_TEXT,
		OUTER_EDGES_CHAIN_COMPLETED_TEXT,
		VEHICLE_POSITIONED_TEXT,
		VEHICLE_ANGLE_TEXT,
		TEXT_COUNT
	};
	std::vector<AbstractText*> m_texts;
	std::vector<ObserverInterface*> m_textObservers;
};
