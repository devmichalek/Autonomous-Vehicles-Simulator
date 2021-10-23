#include "DoubleText.hpp"
#include "ObserverInterface.hpp"
#include "CoreLogger.hpp"
#include "StoppableTimer.hpp"

DoubleText::DoubleText(std::vector<std::string> strings, size_t size) :
	TextAbstract(strings, size),
	m_observer(nullptr)
{
	SetVariableTextInactiveColor();
	m_blendTimer = nullptr;
}

DoubleText::~DoubleText()
{
	delete m_blendTimer;
}

void DoubleText::Reset()
{
	UpdateInternal();
	if (m_observer)
		m_observer->Reset();
	if (m_blendTimer)
		m_blendTimer->MakeTimeout();
}

void DoubleText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 3);

	float consistentX = FontContext::CalculateRow(components[1]), consistentY = 0;
	float variableX = FontContext::CalculateRow(components[2]), variableY = 0;
	consistentY = variableY = FontContext::CalculateColumn(components[0]);
	m_textPositions[CONSISTENT_TEXT] = sf::Vector2f(consistentX, consistentY);
	m_textPositions[VARIABLE_TEXT] = sf::Vector2f(variableX, variableY);

	Update();
}

void DoubleText::SetObserver(ObserverInterface* observer)
{
	if (observer)
	{
		delete m_blendTimer;
		m_blendTimer = new StoppableTimer(0.0, 1.0);
		m_observer = observer;
		Update();
		m_blendTimer->MakeTimeout();
	}
}

sf::Color DoubleText::BlendColors(sf::Color a, sf::Color b, float alpha)
{
	sf::Color result;
	result.r = sf::Uint8((1.f - alpha) * float(a.r) + alpha * float(b.r));
	result.g = sf::Uint8((1.f - alpha) * float(a.g) + alpha * float(b.g));
	result.b = sf::Uint8((1.f - alpha) * float(a.b) + alpha * float(b.b));
	return result;
}

void DoubleText::UpdateInternal()
{
	if (m_observer)
	{
		if (m_observer->Ready())
		{
			m_texts[VARIABLE_TEXT].setString(m_observer->Read());
			m_blendTimer->Reset();
		}

		m_blendTimer->Update();
		m_texts[VARIABLE_TEXT].setFillColor(BlendColors(m_activeColor, m_inactiveColor, float(m_blendTimer->GetValue())));
	}
}

void DoubleText::SetVariableTextInactiveColor()
{
	m_texts[VARIABLE_TEXT].setFillColor(m_inactiveColor);
}

void DoubleText::SetVariableTextActiveColor()
{
	m_texts[VARIABLE_TEXT].setFillColor(m_activeColor);
}
