#pragma once
#include <SFML/Graphics/Text.hpp>

class FontContext final
{
public:

	// Initializes font
	static bool Initialize();

	// Returns global font
	static const sf::Font& GetFont();

	// Returns standard character size multiplied with multiplier
	static unsigned int GetCharacterSize(unsigned int multiplier = 1);

	// Represents x or y ratio
	struct Component
	{
		size_t m_component;
		bool m_revert; // If set to true then axis is interpreted conversely
		Component(size_t component, bool revert = false) :
			m_component(component),
			m_revert(revert)
		{
		}
	};

	// Based on component information calculates text x position
	static float CalculateRow(Component component, unsigned int multiplier = 1);

	// Based on component information calculates text y position
	static float CalculateColumn(Component component, unsigned int multiplier = 1);

private:

	// Global font for all derived classes
	static sf::Font m_font;

	// Standard text character size
	static unsigned int m_characterSize;

	// Field that tells if font is initialized
	static bool m_fontInitialized;
};