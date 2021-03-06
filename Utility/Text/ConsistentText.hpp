#pragma once
#include "AbstractText.hpp"

class ConsistentText :
	public AbstractText
{
public:

	ConsistentText(std::vector<std::string> strings);

	virtual ~ConsistentText()
	{
	}

	void Reset()
	{
	}

	void SetCharacterSize(unsigned int multiplier = 1);

	void SetRotation(float rotation);

	// Sets consistent text and variable text positions
	// First component is used as y position for the text
	// Second component is used as x position for the text
	virtual void SetPosition(std::vector<FontContext::Component> components);

private:

	virtual void UpdateInternal()
	{
	}
};