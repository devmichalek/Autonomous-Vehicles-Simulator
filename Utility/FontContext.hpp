#pragma once
#include <SFML/Graphics/Text.hpp>

class FontContext
{
public:

	// Initializes font
	static bool initialize();

	static const sf::Font& getFont();

	static unsigned int getCharacterSize();

private:

	// Global font for all derived classes
	static sf::Font m_font;

	// Global text character size
	static unsigned int m_characterSize;

	// Field that tells if font is initialized
	static bool m_fontInitialized;
};