#pragma once
#include "DrawableTripleText.hpp"
#include "StoppableTimer.hpp"
#include "CycleTimer.hpp"
#include <array>

template<bool ReadOperations, bool WriteOperations>
class DrawableFilenameText :
	public DrawableTripleText
{
	enum
	{
		STATUS_TEXT = INFORMATION_TEXT + 1
	};

	StoppableTimer m_alphaTimer;

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

	DrawableFilenameText(size_t size = 5);

	~DrawableFilenameText();

	void Reset();

	void SetErrorStatusText(std::string text);

	void SetSuccessStatusText(std::string text);

	void ShowStatusText();

	// Sets consistent text, variable text information text and status text positions
	// First component is used as y position for all texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	// Fourth component is used as x position for information text
	// Fifth component is used as x position for status text
	void SetPosition(std::vector<FontContext::Component> components);

	bool IsReading();

	bool IsWriting();

	bool IsRenaming();

	std::string GetFilename();

	void Capture();

	void UpdateInternal();
};