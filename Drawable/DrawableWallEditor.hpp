#pragma once
#include <limits>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Wall.hpp"
#include "CoreWindow.hpp"

class DrawableWallEditor
{
	bool m_active;

	void deactivate()
	{
		m_active = false;
	}

	enum class Mode { INSERT, GLUED_INSERT, REMOVE };
	Mode m_currentMode;

	Line m_line;
	SegmentVector m_segments;

	bool m_insertSegment;
	bool m_removeSegment;
	sf::Vector2f m_segmentBeggining;

	sf::Font m_font;
	sf::Text m_currentModeText;
	sf::Text m_currentModeActiveText;
	sf::Text m_segmentsCountText;
	sf::Text m_segmentsCountActiveText;
	std::string m_info;

	inline std::string getCurrentMode()
	{
		switch (m_currentMode)
		{
			case Mode::INSERT:
				return "Insert mode";
			case Mode::GLUED_INSERT:
				return "Glued insert mode";
			case Mode::REMOVE:
				return "Remove mode";
		}

		return "Unknown mode";
	}

	inline std::string getSegmentsCount()
	{
		return std::to_string(m_segments.size());
	}

public:
	DrawableWallEditor()
	{
		m_active = true;
		m_currentMode = Mode::INSERT;
		m_segments.reserve(1024);
		m_insertSegment = false;
		m_removeSegment = false;
		m_info = "1 - Insert mode\n2 - Glued insert mode\n3 - Remove mode\nRight mouse button - Perform action";
	}

	~DrawableWallEditor()
	{
	}

	void capture();

	void update();

	void load();

	void draw();

	bool isActive()
	{
		return m_active;
	}

	std::string& getInfo()
	{
		return m_info;
	}
};