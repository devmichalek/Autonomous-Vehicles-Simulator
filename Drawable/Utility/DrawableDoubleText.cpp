#include "DrawableDoubleText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"
#include "ObserverIf.hpp"

DrawableDoubleText::DrawableDoubleText()
{
	// Set fonts
	m_consistentText.setFont(FontContext::GetFont());
	m_variableText.setFont(FontContext::GetFont());

	// Set text colors
	m_consistentText.setFillColor(sf::Color::White);
	SetVariableTextColor();

	// Set texts character size
	m_consistentText.setCharacterSize(FontContext::GetCharacterSize());
	m_variableText.setCharacterSize(FontContext::GetCharacterSize());

	m_observer = nullptr;
}

DrawableDoubleText::~DrawableDoubleText()
{
	delete m_observer;
}

void DrawableDoubleText::SetConsistentText(std::string text)
{
	m_consistentText.setString(text);
}

void DrawableDoubleText::SetVariableText(std::string text)
{
	m_variableText.setString(text);
}

void DrawableDoubleText::SetVariableTextColor(sf::Color color)
{
	m_variableText.setFillColor(color);
}

void DrawableDoubleText::SetPosition(std::array<FontContext::Component, 3> components)
{
	float consistentX = FontContext::CalculateRow(components[0]), consistentY = 0;
	float variableX = FontContext::CalculateRow(components[1]), variableY = 0;
	consistentY = variableY = FontContext::CalculateColumn(components[2]);
	m_consistentPosition = sf::Vector2f(consistentX, consistentY);
	m_variablePosition = sf::Vector2f(variableX, variableY);
	Update();
}

void DrawableDoubleText::SetObserver(ObserverIf* observer)
{
	m_observer = observer;
	Update();
}

void DrawableDoubleText::ResetObserverTimer()
{
	if (m_observer)
		m_observer->Reset();
}

void DrawableDoubleText::Update()
{
	if (m_observer && m_observer->Ready())
		m_variableText.setString(m_observer->Read());

	sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
	m_consistentText.setPosition(m_consistentPosition + viewOffset);
	m_variableText.setPosition(m_variablePosition + viewOffset);
}

void DrawableDoubleText::Draw()
{
	CoreWindow::GetRenderWindow().draw(m_consistentText);
	CoreWindow::GetRenderWindow().draw(m_variableText);
}
