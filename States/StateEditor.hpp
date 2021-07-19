#pragma once
#include <limits>
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "StateAbstract.hpp"
#include "DrawableMath.hpp"
#include "CoreWindow.hpp"
#include "DrawableCar.hpp"

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

	enum class CarMode
	{
		INSERT,
		REMOVE
	} m_carMode;

	Line m_line;
	SegmentVector m_segments;
	bool m_insertSegment;
	bool m_removeSegment;
	sf::Vector2f m_segmentBeggining;

	float m_movement;
	const float m_movementConst;
	const float m_movementMax;
	const float m_movementMin;

	bool m_drawCar;
	DrawableCar m_drawableCar;

	enum class SaveError
	{
		SUCCESS,
		NO_WALLS,
		NO_CAR,
		CANNOT_OPEN_FILE
	};

	bool m_saved;
	bool m_saveKeysPressed;

	sf::Font m_font;
	sf::Text m_wallModeText;
	sf::Text m_wallModeActiveText;
	sf::Text m_wallModeHelpText;
	sf::Text m_carModeText;
	sf::Text m_carModeActiveText;
	sf::Text m_carModeHelpText;
	sf::Text m_segmentsCountText;
	sf::Text m_segmentsCountActiveText;
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
	sf::Text m_saveErrorText;

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

	inline std::string getCarModeString()
	{
		switch (m_carMode)
		{
		case CarMode::INSERT:
			return "Insert mode";
		case CarMode::REMOVE:
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

	inline std::string getCarAngleString()
	{
		long long angle = static_cast<long long>(m_drawableCar.getAngle());
		angle %= 360;
		if (angle < 0)
			angle += 360;
		return std::to_string(angle);
	}

	inline std::string getMovementString()
	{
		long movement = long(m_movement);
		return std::to_string(movement);
	}

	inline std::string getViewOffsetXString()
	{
		sf::Vector2f viewOffset = CoreWindow::getViewOffset();
		return std::to_string(int(viewOffset.x));
	}

	inline std::string getViewOffsetYString()
	{
		sf::Vector2f viewOffset = CoreWindow::getViewOffset();
		return std::to_string(int(viewOffset.y));
	}

	inline void updateSaveActiveText(bool saved)
	{
		m_saved = saved;
		if (m_saved)
		{
			m_saveActiveText.setFillColor(sf::Color::Green);
			m_saveActiveText.setString("Saved");
		}
		else
		{
			m_saveActiveText.setFillColor(sf::Color::Red);
			m_saveActiveText.setString("Not saved");
		}
	}

	inline void updateSaveErrorText(SaveError error = SaveError::SUCCESS)
	{
		m_saveErrorText.setFillColor(sf::Color::Red);
		switch (error)
		{
			case SaveError::SUCCESS:
				m_saveErrorText.setFillColor(sf::Color(0, 0, 0, 0));
				break;
			case SaveError::NO_WALLS:
				m_saveErrorText.setString("Error: No walls specified!");
				break;
			case SaveError::NO_CAR:
				m_saveErrorText.setString("Error: Car is not positioned!");
				break;
			case SaveError::CANNOT_OPEN_FILE:
				m_saveErrorText.setString("Error: Cannot open file!");
				break;
		}
	}

	void updateTextsPosition();

	void save();

public:
	StateEditor(StateEditor&) = delete;

	StateEditor() : m_movementConst(200.0f), m_movementMax(1800.0f), m_movementMin(1.0f)
	{
		m_activeMode = ActiveMode::WALL;
		m_wallMode = WallMode::INSERT;
		m_carMode = CarMode::INSERT;
		m_segments.reserve(1024);
		m_insertSegment = false;
		m_removeSegment = false;
		m_movement = m_movementConst * 3;
		m_drawCar = false;
		m_saved = false;
		m_saveKeysPressed = false;
	}

	~StateEditor()
	{
	}

	void capture();

	void update();

	void load();

	void draw();
};
