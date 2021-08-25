#pragma once
#include "DrawableDoubleText.hpp"

class Observer;

class DrawableTripleText :
	public DrawableDoubleText
{
public:

	DrawableTripleText(size_t size = 3);

	virtual ~DrawableTripleText();

	// Sets consistent text, variable text and information text positions
	// First component is used as y position for all texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	// Fourth component is used as x position for information text
	virtual void SetPosition(std::vector<FontContext::Component> components);

protected:

	enum
	{
		INFORMATION_TEXT = VARIABLE_TEXT + 1
	};
};