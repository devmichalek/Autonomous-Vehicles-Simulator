#pragma once
#include "FontContext.hpp"
#include "CoreWindow.hpp"
#include "CoreLogger.hpp"

class TextAbstract
{
protected:

	const size_t m_size;
	std::vector<sf::Text> m_texts;
	std::vector<sf::Vector2f> m_textPositions;
	inline static const sf::Color m_activeColor = sf::Color(0xFF, 0xAA, 0x1D, 0xFF);
	inline static const sf::Color m_inactiveColor = sf::Color(0xC0, 0xC0, 0xC0, 0xFF);

	// Validates number of components
	void ValidateNumberOfComponents(std::vector<FontContext::Component>& components, const size_t requiredSize)
	{
		if (components.size() != requiredSize)
		{
			components.resize(requiredSize, FontContext::Component(0));
			CoreLogger::PrintError("Invalid number of components specified for the text!");
		}
	}

	TextAbstract(std::vector<std::string>& strings, size_t size) :
		m_size(size)
	{
		m_texts.resize(m_size);
		m_textPositions.resize(m_size);

		for (auto& text : m_texts)
		{
			text.setFont(FontContext::GetFont());
			text.setFillColor(sf::Color::White);
			text.setCharacterSize(FontContext::GetCharacterSize());
		}

		strings.resize(m_size, "");
		for (size_t i = 0; i < m_size; ++i)
		{
			if (!strings[i].empty())
				m_texts[i].setString(strings[i]);
		}
	}

	// Update internal implementation
	virtual void UpdateInternal() = 0;

public:

	virtual ~TextAbstract()
	{
	}

	// Resets implementation defined fields
	virtual void Reset() = 0;

	// Sets positions of texts
	virtual void SetPosition(std::vector<FontContext::Component> components) = 0;
	
	// Updates texts positions and calls internal update implementation
	void Update()
	{
		UpdateInternal();
		for (size_t i = 0; i < m_size; ++i)
			m_texts[i].setPosition(m_textPositions[i]);
	}

	// Draws texts
	void Draw()
	{
		for (size_t i = 0; i < m_size; ++i)
			CoreWindow::DrawForeground(m_texts[i]);
	}
};