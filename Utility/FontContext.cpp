#include "FontContext.hpp"
#include "CoreWindow.hpp"

sf::Font FontContext::m_font;
unsigned int FontContext::m_characterSize = 0;
bool FontContext::m_fontInitialized = false;

bool FontContext::initialize()
{
	if (m_fontInitialized)
	{
		// Font is already initialized
		return true;
	}

	std::string filename = "Data/consola.ttf";
	if (!m_font.loadFromFile(filename))
	{
		// Error cannot open given file
		return false;
	}

	// Calculate character size
	m_characterSize = CoreWindow::getSize().x / 116;

	// Font was initialized correctly
	m_fontInitialized = true;
	return true;
}

const sf::Font& FontContext::getFont()
{
	return m_font;
}

unsigned int FontContext::getCharacterSize()
{
	return m_characterSize;
}