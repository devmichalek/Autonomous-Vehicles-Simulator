#include "DrawableTripleText.hpp"
#include "ObserverIf.hpp"
#include "CoreLogger.hpp"

DrawableTripleText::DrawableTripleText(std::vector<std::string> strings, size_t size) :
	DrawableDoubleText(strings, size)
{
}

DrawableTripleText::~DrawableTripleText()
{
}

void DrawableTripleText::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 4);

	float informationX = FontContext::CalculateRow(components[3]);
	float informationY = FontContext::CalculateColumn(components[0]);
	m_textPositions[INFORMATION_TEXT] = sf::Vector2f(informationX, informationY);
	components.pop_back();

	DrawableDoubleText::SetPosition(components);

	Update();
}
