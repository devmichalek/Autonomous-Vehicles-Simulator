#include "TripleText.hpp"
#include "CoreLogger.hpp"

TripleText::TripleText(std::vector<std::string> strings, size_t size) :
	DoubleText(strings, size)
{
}

TripleText::~TripleText()
{
}

void TripleText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 4);

	float informationX = FontContext::CalculateRow(components[3]);
	float informationY = FontContext::CalculateColumn(components[0]);
	m_textPositions[INFORMATION_TEXT] = sf::Vector2f(informationX, informationY);
	components.pop_back();

	DoubleText::SetPosition(components);

	Update();
}
