#pragma once
#include "AbstractText.hpp"

class ObserverInterface;
class TimerAbstract;

class DoubleText :
	public AbstractText
{
public:

	// <Consistent text> <Variable text>
	DoubleText(std::vector<std::string> strings, size_t size = 2);

	virtual ~DoubleText();

	// In case of existing observer sets text string and resets observer
	void Reset() override;

	// Sets consistent text and variable text positions
	// First component is used as y position for both texts
	// Second component is used as x position for consistent text
	// Third component is used as x position for variable text
	virtual void SetPosition(std::vector<FontContext::Component> components) override;

	// Sets observer for variable text string
	void SetObserver(ObserverInterface* observer);

	// Returns the result of two blended colors based on alpha factor
	sf::Color BlendColors(sf::Color a, sf::Color b, float alpha);

protected:

	// If observer is ready text string is set
	virtual void UpdateInternal() override;

	// Sets second (variable) text inactive color
	void SetVariableTextInactiveColor();

	// Sets second (variable) text active color
	void SetVariableTextActiveColor();

	enum
	{
		CONSISTENT_TEXT,
		VARIABLE_TEXT
	};

	ObserverInterface* m_observer;
	TimerAbstract* m_blendTimer;
};