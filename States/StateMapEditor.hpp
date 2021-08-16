#pragma once
#include "StateInterface.hpp"
#include "DrawableCar.hpp"
#include "DrawableDoubleText.hpp"
#include "DrawableTripleText.hpp"
#include "DrawableFilenameText.hpp"
#include "DrawableBuilder.hpp"
#include "CycleTimer.hpp"
#include <functional>

class StateMapEditor final : public StateInterface
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

	Line m_line;
	EdgeVector m_edges;
	bool m_insertEdge;
	bool m_removeEdge;
	sf::Vector2f m_edgeBeggining;

	CycleTimer m_movementTimer;
	bool m_upToDate;

	bool m_carPositioned;
	DrawableCar m_drawableCar;

	DrawableBuilder m_builder;
	bool m_spaceKeyPressed;

	DrawableTripleText m_activeModeText;
	DrawableTripleText m_movementText;
	DrawableTripleText m_viewOffsetXText;
	DrawableTripleText m_viewOffsetYText;
	DrawableFilenameText<true, true> m_filenameText;
	DrawableTripleText m_edgeSubmodeText;
	DrawableDoubleText m_edgeCountText;
	DrawableTripleText m_carSubmodeText;
	DrawableTripleText m_carAngleText;
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
