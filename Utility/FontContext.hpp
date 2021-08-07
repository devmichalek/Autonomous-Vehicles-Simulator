#pragma once
#include <SFML/Graphics/Text.hpp>

class FontContext
{
public:

	// Initializes font
	static bool Initialize();

	// Returns global font
	static const sf::Font& GetFont();

	// Returns standard character size multiplied with multiplier
	static unsigned int GetCharacterSize(size_t multiplier = 1);

private:

	// Global font for all derived classes
	static sf::Font m_font;

	// Standard text character size
	static unsigned int m_characterSize;

	// Field that tells if font is initialized
	static bool m_fontInitialized;
};