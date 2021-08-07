#pragma once
#include "StateInterface.hpp"
#include "DrawableCar.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableTripleTextStatus.hpp"
#include "CycleTimer.hpp"

class StateEditor : public StateInterface
{
	enum class ActiveMode
	{
		EDGE,
		CAR
	} m_activeMode;
	std::map<ActiveMode, std::string> m_activeModeMap;

	enum class EdgeSubmode
	{
		GLUED_INSERT,
		REMOVE
	} m_edgeSubmode;
	std::map<EdgeSubmode, std::string> m_edgeSubmodeMap;

	enum class CarSubmode
	{
		INSERT,
		REMOVE
	} m_carSubmode;
	std::map<CarSubmode, std::string> m_carSubmodeMap;

	enum class SaveStatus
	{
		UP_TO_DATE,
		OUT_OF_DATE,
		ERROR_NO_EDGES_POSITIONED,
		ERROR_NO_CAR_POSITIONED,
		ERROR_CANNOT_OPEN_FILE
	} m_saveStatus;
	std::map<SaveStatus, std::tuple<std::string, std::string, sf::Color>> m_saveStatusMap;

	Line m_line;
	EdgeVector m_edges;
	size_t m_numberOfEdges;
	bool m_insertEdge;
	bool m_removeEdge;
	sf::Vector2f m_edgeBeggining;

	CycleTimer m_movementTimer;

	bool m_drawCar;
	DrawableCar m_drawableCar;

	bool m_spaceKeyPressed;
	bool m_saveKeysPressed;

	DrawableTripleText m_activeModeText;
	DrawableTripleText m_movementText;
	DrawableDoubleText m_viewOffsetXText;
	DrawableDoubleText m_viewOffsetYText;
	DrawableTripleTextStatus m_saveText;
	DrawableTripleText m_edgeSubmodeText;
	DrawableDoubleText m_edgeCountText;
	DrawableTripleText m_carSubmodeText;
	DrawableTripleText m_carAngleText;
	std::vector<std::function<std::string()>> m_textFunctions;

	void setActiveMode(ActiveMode);

public:
	StateEditor(StateEditor&) = delete;

	StateEditor() :
		m_movementTimer(600.0, 1800.0, 1.0, 200.0)
	{
		m_activeMode = ActiveMode::EDGE;
		m_edgeSubmode = EdgeSubmode::GLUED_INSERT;
		m_carSubmode = CarSubmode::INSERT;
		m_saveStatus = SaveStatus::OUT_OF_DATE;
		m_line[0].color = sf::Color::White;
		m_line[1].color = sf::Color::White;
		m_edges.reserve(1024);
		m_numberOfEdges = 0;
		m_insertEdge = false;
		m_removeEdge = false;
		m_drawCar = false;
		m_spaceKeyPressed = false;
		m_saveKeysPressed = false;
	}

	~StateEditor()
	{
	}

	void capture();

	void update();

	bool load();

	void draw();
};
