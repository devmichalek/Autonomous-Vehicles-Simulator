#pragma once
#include "DrawableStatusText.hpp"
#include "ContinuousTimer.hpp"
#include <array>

template<bool ReadOperations, bool WriteOperations>
class DrawableFilenameText :
	public DrawableStatusText
{
public:

	// <Consistent text> <Variable text> <Information text> <Status text>
	// <Variable text> is managed by this class
	DrawableFilenameText(std::string filenameDummy);

	~DrawableFilenameText();

	// Reset various class fields like states, actions, variable text string etc.
	void Reset();

	// Returns true if reading action was invoked by caller
	// Once true is returned action is set to default
	bool IsReading();

	// Returns true if writing action was invoked by caller
	// Once true is returned action is set to default
	bool IsWriting();

	// Returns true if renaming action is taking place until it is interrupter
	bool IsRenaming() const;

	// Returns current variable text string - filename
	std::string GetFilename() const;

	// Captures key events
	void Capture();

private:

	// Converts SFML key to ASCII key
	// Returns conversion status + ASCII representation
	std::pair<bool, char> ConvertKeyToAscii(sf::Keyboard::Key eventKey);

	// Function called on key pressed event
	void OnControlKeyPressedEvent(sf::Keyboard::Key eventKey);

	// Function called on key released event
	void OnControlKeyReleasedEvent(sf::Keyboard::Key eventKey);

	// Returns internal information text string
	std::string GetInformationString() const;

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
	ContinuousTimer m_pressedBackspaceKeyTimer;

	std::map<sf::Keyboard::Key, bool> m_pressedFilenameKeys;
	std::string m_filename;
	const std::string m_filenameDummy;
	const size_t m_maxFilenameLength;
};