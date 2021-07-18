#pragma once
#include <limits>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "StateAbstract.hpp"
#include "Wall.hpp"
#include "CoreWindow.hpp"

class StateEditor : public StateAbstract
{
	enum class ActiveMode
	{
		WALL,
		CAR
	} m_activeMode;

	enum class WallMode
	{
		INSERT,
		GLUED_INSERT,
		REMOVE
	} m_wallMode;

	Line m_line;
	SegmentVector m_segments;
	bool m_insertSegment;
	bool m_removeSegment;
	sf::Vector2f m_segmentBeggining;

	float m_movement;
	const float m_movementConst;
	const float m_movementMax;
	const float m_movementMin;

	sf::Font m_font;
	sf::Text m_wallModeText;
	sf::Text m_wallModeActiveText;
	sf::Text m_wallModeHelpText;
	sf::Text m_segmentsCountText;
	sf::Text m_segmentsCountActiveText;
	sf::Text m_activeModeText;
	sf::Text m_activeModeActiveText;
	sf::Text m_activeModeHelpText;
	sf::Text m_movementText;
	sf::Text m_movementActiveText;
	sf::Text m_movementHelpText;

	std::string m_info;

	inline std::string getWallModeString()
	{
		switch (m_wallMode)
		{
			case WallMode::INSERT:
				return "Insert mode";
			case WallMode::GLUED_INSERT:
				return "Glued insert mode";
			case WallMode::REMOVE:
				return "Remove mode";
		}

		return "Unknown mode";
	}

	inline std::string getActiveModeString()
	{
		switch (m_activeMode)
		{
			case ActiveMode::CAR:
				return "Car mode";
			case ActiveMode::WALL:
				return "Wall mode";
		}

		return "Unknown mode";
	}

	inline std::string getSegmentsCountString()
	{
		return std::to_string(m_segments.size());
	}

	inline std::string getMovementString()
	{
		long movement = long(m_movement);
		return std::to_string(movement);
	}

	void updateTextsPosition();

public:
	StateEditor(StateEditor&) = delete;

	StateEditor() : m_movementConst(200.0f), m_movementMax(1800.0f), m_movementMin(1.0f)
	{
		m_activeMode = ActiveMode::WALL;
		m_wallMode = WallMode::INSERT;
		m_segments.reserve(1024);
		m_insertSegment = false;
		m_removeSegment = false;
		m_info = "1 - Insert mode\n2 - Glued insert mode\n3 - Remove mode\nRight mouse button - Perform action";
		m_movement = m_movementConst * 3;
	}

	~StateEditor()
	{
	}

	void capture();

	void update();

	void load();

	void draw();
};
