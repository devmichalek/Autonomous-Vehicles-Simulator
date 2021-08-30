#pragma once
#include "DrawableStatusText.hpp"
#include "CycleTimer.hpp"
#include <array>

template<bool ReadOperations, bool WriteOperations>
class DrawableFilenameText :
	public DrawableStatusText
{
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

	std::string GetInformationString();

public:

	DrawableFilenameText();

	~DrawableFilenameText();

	void Reset();

	bool IsReading();

	bool IsWriting();

	bool IsRenaming();

	std::string GetFilename();

	void Capture();
};