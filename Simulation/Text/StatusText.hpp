#pragma once
#include "TripleText.hpp"
#include "StoppableTimer.hpp"

class Observer;

class StatusText :
	public TripleText
{
public:

	// <Consistent text> <Variable text> <Information text> <Status text>
	StatusText(std::vector<std::string> strings);

	virtual ~StatusText();

	// Make timeout on alpha timer and calls reset parent function version
	virtual void Reset();

	// Sets error status text, text color is red
	void SetErrorStatusText(std::string text);

	// Sets success status text, text color is green
	void SetSuccessStatusText(std::string text);

	// Calls reset on alpha timer
	void ShowStatusText();

	// Sets consistent text, variable text information text and status text positions
	// First component is used as y position for all texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	// Fourth component is used as x position for information text
	// Fifth component is used as x position for status text
	virtual void SetPosition(std::vector<FontContext::Component> components);

	// Calls update on alpha timer (value decreases), calls parent update function
	void UpdateInternal();

protected:

	enum
	{
		STATUS_TEXT = INFORMATION_TEXT + 1
	};

	// Timer for alpha channel status text color
	StoppableTimer m_alphaTimer;
};