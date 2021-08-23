#pragma once
#include "DrawableTripleText.hpp"
#include "StoppableTimer.hpp"
#include "CycleTimer.hpp"
#include <array>

template<bool ReadOperations, bool WriteOperations>
class DrawableFilenameText :
	public DrawableTripleText
{
	sf::Text m_statusText;
	sf::Vector2f m_statusPosition;
	StoppableTimer m_alphaTimer;
	DrawableTripleText m_drawableTripleText;

	// Actions
	enum
	{
		READING_ACTION,
		WRITING_ACTION,
		RENAMING_ACTION,
		ACTION_COUNT
	};
	std::array<bool, ACTION_COUNT> m_activeActions;

	std::map<sf::Keyboard::Key, bool> m_pressedControlKeys;
	CycleTimer m_pressedBackspaceKeyTimer;
	
	std::map<sf::Keyboard::Key, bool> m_pressedFilenameKeys;
	std::string m_filename;
	static inline const std::string m_filenameDummy = "dummy.txt";
	const size_t m_maxFilenameLength;

	std::pair<bool, char> ConvertKeyToAscii(sf::Keyboard::Key eventKey);

	void OnControlKeyPressedEvent(sf::Keyboard::Key eventKey);

	void OnControlKeyReleasedEvent(sf::Keyboard::Key eventKey);

public:

	DrawableFilenameText();

	~DrawableFilenameText();

	void Reset();

	void SetErrorStatusText(std::string text);

	void SetSuccessStatusText(std::string text);

	void ShowStatusText();

	// Sets consistent text, variable text information text and status text positions
	// First component is used as x position for consistent text
	// Second component is used as x position for variable text
	// Third component is used as x position for information text
	// Fourth component is used as x position for status text
	// Fifth component is used as y position for all texts
	void SetPosition(std::array<FontContext::Component, 5> components);

	bool IsReading();

	bool IsWriting();

	bool IsRenaming();

	std::string GetFilename();

	void Capture();

	void Update();

	void Draw();
};