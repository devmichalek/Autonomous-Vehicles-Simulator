#pragma once
#include "DrawableTripleText.hpp"
#include "StoppableTimer.hpp"

class Observer;

class DrawableStatusText :
	public DrawableTripleText
{
public:

	DrawableStatusText(std::vector<std::string> strings);

	virtual ~DrawableStatusText();

	virtual void Reset();

	void SetErrorStatusText(std::string text);

	void SetSuccessStatusText(std::string text);

	void ShowStatusText();

	// Sets consistent text, variable text information text and status text positions
	// First component is used as y position for all texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	// Fourth component is used as x position for information text
	// Fifth component is used as x position for status text
	virtual void SetPosition(std::vector<FontContext::Component> components);

	void UpdateInternal();

protected:

	enum
	{
		STATUS_TEXT = INFORMATION_TEXT + 1
	};

	StoppableTimer m_alphaTimer;
};