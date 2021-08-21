#include "FontContext.hpp"
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"

sf::Font FontContext::m_font;
unsigned int FontContext::m_characterSize = 0;
bool FontContext::m_fontInitialized = false;

bool FontContext::Initialize()
{
	if (m_fontInitialized)
	{
		CoreLogger::PrintError("Font is already initialized");
		return true;
	}

	std::string filename = "Data/consola.ttf";
	if (!m_font.loadFromFile(filename))
	{
		CoreLogger::PrintError("Cannot open \"" + filename + "\"");
		return false;
	}

	// Calculate character size
	m_characterSize = unsigned(CoreWindow::GetSize().x / 116.0f);

	// Font was initialized correctly
	m_fontInitialized = true;

	CoreLogger::PrintSuccess("FontContext initialized correctly");
	return true;
}

const sf::Font& FontContext::GetFont()
{
	return m_font;
}

unsigned int FontContext::GetCharacterSize(unsigned int multiplier)
{
	if (multiplier > 6)
		return m_characterSize;

	return m_characterSize * multiplier;
}

float FontContext::CalculateRow(FontContext::Component component, unsigned int multiplier)
{
	float screenWidth = CoreWindow::GetSize().x;
	float result = screenWidth * (0.003f + float(component.m_component) * 0.022f * float(multiplier));

	if (component.m_revert)
		return screenWidth - result;

	return result;
}

float FontContext::CalculateColumn(FontContext::Component component, unsigned int multiplier)
{
	float screenHeight = CoreWindow::GetSize().y;
	float result = screenHeight * (0.003f + float(component.m_component) * 0.022f * float(multiplier));

	if (component.m_revert)
		return screenHeight - result;

	return result;
}
