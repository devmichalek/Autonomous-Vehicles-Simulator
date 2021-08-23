#include "DrawableTripleText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"
#include "ObserverIf.hpp"

DrawableTripleText::DrawableTripleText() :
	DrawableDoubleText()
{
	// Set font
	m_informationText.setFont(FontContext::GetFont());

	// Set color
	m_informationText.setFillColor(sf::Color::White);

	// Set character size
	m_informationText.setCharacterSize(FontContext::GetCharacterSize());
}

DrawableTripleText::~DrawableTripleText()
{
}

void DrawableTripleText::SetInformationText(std::string text)
{
	m_informationText.setString(text);
}

void DrawableTripleText::SetPosition(std::array<FontContext::Component, 4> components)
{
	float consistentX = FontContext::CalculateRow(components[0]), consistentY = 0;
	float variableX = FontContext::CalculateRow(components[1]), variableY = 0;
	float informationX = FontContext::CalculateRow(components[2]), informationY = 0;
	consistentY = variableY = informationY = FontContext::CalculateColumn(components[3]);
	m_consistentPosition = sf::Vector2f(consistentX, consistentY);
	m_variablePosition = sf::Vector2f(variableX, variableY);
	m_informationPosition = sf::Vector2f(informationX, informationY);
	Update();
}

void DrawableTripleText::Update()
{
	if (m_observer && m_observer->Ready())
		m_variableText.setString(m_observer->Read());

	sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
	m_consistentText.setPosition(m_consistentPosition + viewOffset);
	m_variableText.setPosition(m_variablePosition + viewOffset);
	m_informationText.setPosition(m_informationPosition + viewOffset);
}

void DrawableTripleText::Draw()
{
	CoreWindow::GetRenderWindow().draw(m_consistentText);
	CoreWindow::GetRenderWindow().draw(m_variableText);
	CoreWindow::GetRenderWindow().draw(m_informationText);
}