#pragma once
#include "StateAbstract.hpp"
#include "DrawableCar.hpp"
#include "DrawableFinishLine.hpp"
#include <SFML/Graphics/Text.hpp>

class StateEditor : public StateAbstract
{
	enum class ActiveMode
	{
		WALL,
		CAR,
		FINISH_LINE
	} m_activeMode;
	std::map<ActiveMode, std::string> m_activeModeMap;

	enum class WallSubmode
	{
		INSERT,
		GLUED_INSERT,
		REMOVE
	} m_wallSubmode;
	std::map<WallSubmode, std::string> m_wallSubmodeMap;

	enum class CarSubmode
	{
		INSERT,
		REMOVE
	} m_carSubmode;
	std::map<CarSubmode, std::string> m_carSubmodeMap;

	enum class FinishLineSubmode
	{
		INSERT
	} m_finishLineSubmode;
	std::map<FinishLineSubmode, std::string> m_finishLineSubmodeMap;

	enum class SaveStatus
	{
		UP_TO_DATE,
		OUT_OF_DATE,
		ERROR_NO_WALLS_POSITIONED,
		ERROR_NO_CAR_POSITIONED,
		ERROR_NO_FINISH_LINE_POSITIONED,
		ERROR_CANNOT_OPEN_FILE
	} m_saveStatus;
	std::map<SaveStatus, std::tuple<std::string, std::string, sf::Color>> m_saveStatusMap;

	Line m_line;
	SegmentVector m_walls;
	bool m_insertWall;
	bool m_removeWall;
	sf::Vector2f m_wallBeggining;

	float m_movement;
	const float m_movementConst;
	const float m_movementMax;
	const float m_movementMin;

	bool m_drawCar;
	DrawableCar m_drawableCar;

	bool m_drawFinishLine;
	bool m_insertFinishLine;
	sf::Vector2f m_finishLineBeggining;
	DrawableFinishLine m_drawableFinishLine;

	bool m_saveKeysPressed;
	float m_saveStatusAlpha;
	const float m_saveStatusAlphaConst;
	const float m_saveStatusAlphaMax;
	const float m_saveStatusAlphaMin;

	sf::Font m_font;
	sf::Text m_wallSubmodeText;
	sf::Text m_wallSubmodeActiveText;
	sf::Text m_wallSubmodeHelpText;
	sf::Text m_carSubmodeText;
	sf::Text m_carSubmodeActiveText;
	sf::Text m_carSubmodeHelpText;
	sf::Text m_finishLineSubmodeText;
	sf::Text m_finishLineSubmodeActiveText;
	sf::Text m_finishLineSubmodeHelpText;
	sf::Text m_wallCountText;
	sf::Text m_wallCountActiveText;
	sf::Text m_carAngleText;
	sf::Text m_carAngleActiveText;
	sf::Text m_carAngleHelpText;
	sf::Text m_activeModeText;
	sf::Text m_activeModeActiveText;
	sf::Text m_activeModeHelpText;
	sf::Text m_movementText;
	sf::Text m_movementActiveText;
	sf::Text m_movementHelpText;
	sf::Text m_viewOffsetXText;
	sf::Text m_viewOffsetXActiveText;
	sf::Text m_viewOffsetYText;
	sf::Text m_viewOffsetYActiveText;
	sf::Text m_saveText;
	sf::Text m_saveActiveText;
	sf::Text m_saveHelpText;
	sf::Text m_saveStatusText;

	void setWallCountActiveText();
	void setCarAngleActiveText();
	void setMovementActiveText();
	void setViewOffsetXActiveText();
	void setViewOffsetYActiveText();
	void setUpToDate();
	void setOutOfDate();
	void updateTextsPosition();
	void save();

public:
	StateEditor(StateEditor&) = delete;

	StateEditor() :
		m_movementConst(200.0f),
		m_movementMax(1800.0f),
		m_movementMin(1.0f),
		m_saveStatusAlphaConst(75.0f),
		m_saveStatusAlphaMax(255.0f),
		m_saveStatusAlphaMin(0.0f)
	{
		m_activeMode = ActiveMode::WALL;
		m_wallSubmode = WallSubmode::INSERT;
		m_carSubmode = CarSubmode::INSERT;
		m_finishLineSubmode = FinishLineSubmode::INSERT;
		m_saveStatus = SaveStatus::OUT_OF_DATE;
		m_line[0].color = sf::Color::White;
		m_line[1].color = sf::Color::White;
		m_walls.reserve(1024);
		m_insertWall = false;
		m_removeWall = false;
		m_movement = m_movementConst * 3;
		m_drawCar = false;
		m_drawFinishLine = false;
		m_insertFinishLine = false;
		m_saveKeysPressed = false;
		m_saveStatusAlpha = m_saveStatusAlphaMin;
	}

	~StateEditor()
	{
	}

	void capture();

	void update();

	void load();

	void draw();
};
