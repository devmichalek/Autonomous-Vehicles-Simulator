#include "FontContext.hpp"
#include "CoreWindow.hpp"
#include "CoreConsoleLogger.hpp"

sf::Font FontContext::m_font;
unsigned int FontContext::m_characterSize = 0;
bool FontContext::m_fontInitialized = false;

bool FontContext::Initialize()
{
	if (m_fontInitialized)
	{
		CoreConsoleLogger::PrintError("Font is already initialized");
		return true;
	}

	std::string filename = "Data/consola.ttf";
	if (!m_font.loadFromFile(filename))
	{
		CoreConsoleLogger::PrintError("Cannot open \"" + filename + "\"");
		return false;
	}

	// Calculate character size
	m_characterSize = CoreWindow::GetSize().x / 116;

	// Font was initialized correctly
	m_fontInitialized = true;

	CoreConsoleLogger::PrintSuccess("FontContext initialized correctly");
	return true;
}

const sf::Font& FontContext::GetFont()
{
	return m_font;
}

unsigned int FontContext::GetCharacterSize(size_t multiplier)
{
	if (multiplier > 6)
		return m_characterSize;

	return m_characterSize * multiplier;
}