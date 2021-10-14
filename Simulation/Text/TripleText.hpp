#pragma once
#include "DoubleText.hpp"

class Observer;

class TripleText :
	public DoubleText
{
public:

	// <Consistent text> <Variable text> <Information text>
	TripleText(std::vector<std::string> strings, size_t size = 3);

	virtual ~TripleText();

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