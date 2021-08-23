#include "DrawableFilenameText.hpp"
#include "FontContext.hpp"
#include "CoreWindow.hpp"

template<bool ReadOperations, bool WriteOperations>
std::pair<bool, char> DrawableFilenameText<ReadOperations, WriteOperations>::ConvertKeyToAscii(sf::Keyboard::Key eventKey)
{
	if (eventKey >= sf::Keyboard::A && eventKey <= sf::Keyboard::Z)
	{
		if (m_pressedControlKeys[sf::Keyboard::LShift] || m_pressedControlKeys[sf::Keyboard::RShift])
			return std::pair(true, char(static_cast<unsigned>(eventKey) + 65));
		return std::pair(true, char(static_cast<unsigned>(eventKey) + 97));
	}

	if (eventKey >= sf::Keyboard::Num0 && eventKey <= sf::Keyboard::Num9)
		return std::pair(true, char(static_cast<unsigned>(eventKey) + 22));

	if (eventKey >= sf::Keyboard::Numpad0 && eventKey <= sf::Keyboard::Numpad9)
		return std::pair(true, char(static_cast<unsigned>(eventKey) - 27));
	
	if (eventKey == sf::Keyboard::Period)
		return std::pair(true, '.');

	return std::pair(false, char(0));
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::OnControlKeyPressedEvent(sf::Keyboard::Key eventKey)
{
	if (m_activeActions[RENAMING_ACTION])
	{
		auto result = m_pressedFilenameKeys.find(eventKey);
		if (result != m_pressedFilenameKeys.end() && !result->second)
		{
			result->second = true;
			if (m_filename.size() < m_maxFilenameLength)
			{
				m_filename += ConvertKeyToAscii(eventKey).second;
				m_drawableTripleText.SetVariableText(m_filename);
			}
		}
		else
		{
			result = m_pressedControlKeys.find(eventKey);
			if (result != m_pressedControlKeys.end())
			{
				if (result->first == sf::Keyboard::BackSpace)
				{
					if (!m_filename.empty() && m_pressedBackspaceKeyTimer.Increment())
					{
						m_filename.erase(m_filename.begin() + (m_filename.size() - 1));
						m_drawableTripleText.SetVariableText(m_filename);
					}
				}
				else if (result->first == sf::Keyboard::LShift || result->first == sf::Keyboard::RShift)
					result->second = true;
				else if (result->first == sf::Keyboard::Escape)
				{
					if (m_filename.empty())
					{
						m_filename = m_filenameDummy;
						m_drawableTripleText.SetVariableText(m_filename);
					}
					m_pressedControlKeys[sf::Keyboard::Escape] = false;
					m_drawableTripleText.SetVariableTextColor();
					m_activeActions[RENAMING_ACTION] = false;
				}
			}
		}
	}
	else
	{
		auto result = m_pressedControlKeys.find(eventKey);
		if (result != m_pressedControlKeys.end() && !result->second)
			result->second = true;

		if (m_pressedControlKeys[sf::Keyboard::LControl] || m_pressedControlKeys[sf::Keyboard::RControl])
		{
			if (m_pressedControlKeys[sf::Keyboard::O])
			{
				m_activeActions[READING_ACTION] = true;
				m_pressedControlKeys[sf::Keyboard::O] = false;
				m_pressedControlKeys[sf::Keyboard::LControl] = false;
				m_pressedControlKeys[sf::Keyboard::RControl] = false;
			}
			else if (m_pressedControlKeys[sf::Keyboard::S])
			{
				m_activeActions[WRITING_ACTION] = true;
				m_pressedControlKeys[sf::Keyboard::S] = false;
				m_pressedControlKeys[sf::Keyboard::LControl] = false;
				m_pressedControlKeys[sf::Keyboard::RControl] = false;
			}
			else if (m_pressedControlKeys[sf::Keyboard::R])
			{
				m_activeActions[RENAMING_ACTION] = true;
				m_pressedControlKeys[sf::Keyboard::R] = false;
				m_pressedControlKeys[sf::Keyboard::LControl] = false;
				m_pressedControlKeys[sf::Keyboard::RControl] = false;
				m_drawableTripleText.SetVariableTextColor(sf::Color::Yellow);
			}
		}
	}
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::OnControlKeyReleasedEvent(sf::Keyboard::Key eventKey)
{
	auto result = m_pressedFilenameKeys.find(eventKey);
	if (result != m_pressedFilenameKeys.end())
		result->second = false;
	else
	{
		result = m_pressedControlKeys.find(eventKey);
		if (result != m_pressedControlKeys.end())
			result->second = false;
	}
}

template<bool ReadOperations, bool WriteOperations>
DrawableFilenameText<ReadOperations, WriteOperations>::DrawableFilenameText() :
	m_alphaTimer(255.0, 255.0, 0.0, 75.0),
	m_maxFilenameLength(18),
	m_pressedBackspaceKeyTimer(0.0, 1.0, 0.0, 5000.0)
{
	// Set font
	m_statusText.setFont(FontContext::GetFont());

	// Set color
	m_statusText.setFillColor(sf::Color::Red);

	// Set character size
	m_statusText.setCharacterSize(FontContext::GetCharacterSize());

	m_drawableTripleText.SetConsistentText("Filename:");

	std::string informationString = "| ";
	if (WriteOperations)
		informationString += "[Ctrl]+[S] ";
	if (ReadOperations)
		informationString += "[Ctrl]+[O] ";
	informationString += "[Ctrl]+[R] [Esc]";
	m_drawableTripleText.SetInformationText(informationString);

	for (size_t i = 0; i < ACTION_COUNT; ++i)
		m_activeActions[i] = false;

	// Set allowed control keys
	m_pressedControlKeys[sf::Keyboard::LControl] = false;
	m_pressedControlKeys[sf::Keyboard::RControl] = false;
	m_pressedControlKeys[sf::Keyboard::O] = false;
	m_pressedControlKeys[sf::Keyboard::S] = false;
	m_pressedControlKeys[sf::Keyboard::R] = false;
	m_pressedControlKeys[sf::Keyboard::Escape] = false;
	m_pressedControlKeys[sf::Keyboard::BackSpace] = false;

	for (size_t i = 0; i < size_t(sf::Keyboard::KeyCount); ++i)
	{
		auto value = ConvertKeyToAscii(sf::Keyboard::Key(i));
		if (value.first)
			m_pressedFilenameKeys[sf::Keyboard::Key(i)] = false;
	}

	// Reset filename
	m_filename = m_filenameDummy;
	m_drawableTripleText.SetVariableText(m_filename);
}

template<bool ReadOperations, bool WriteOperations>
DrawableFilenameText<ReadOperations, WriteOperations>::~DrawableFilenameText()
{
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::Reset()
{
	for (size_t i = 0; i < ACTION_COUNT; ++i)
		m_activeActions[i] = false;
	for (auto& pressedControlKey : m_pressedControlKeys)
		pressedControlKey.second = false;
	for (auto& pressedFilenameKey : m_pressedFilenameKeys)
		pressedFilenameKey.second = false;
	m_filename = m_filenameDummy;
	m_drawableTripleText.SetVariableText(m_filename);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetErrorStatusText(std::string text)
{
	m_statusText.setString(text);
	m_statusText.setFillColor(sf::Color::Red);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetSuccessStatusText(std::string text)
{
	m_statusText.setString(text);
	m_statusText.setFillColor(sf::Color::Green);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::ShowStatusText()
{
	m_alphaTimer.Reset();
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetPosition(std::array<FontContext::Component, 5> components)
{
	float statusX = FontContext::CalculateRow(components[3]);
	float statusY = FontContext::CalculateColumn(components[4]);
	m_drawableTripleText.SetPosition({ components[0], components[1], components[2], components[4] });
	m_statusPosition = sf::Vector2f(statusX, statusY);
	Update();
}

template<bool ReadOperations, bool WriteOperations>
bool DrawableFilenameText<ReadOperations, WriteOperations>::IsReading()
{
	if (!ReadOperations)
		return false;
	bool reading = m_activeActions[READING_ACTION];
	m_activeActions[READING_ACTION] = false;
	return reading;
}

template<bool ReadOperations, bool WriteOperations>
bool DrawableFilenameText<ReadOperations, WriteOperations>::IsWriting()
{
	if (!WriteOperations)
		return false;
	bool writing = m_activeActions[WRITING_ACTION];
	m_activeActions[WRITING_ACTION] = false;
	return writing;
}

template<bool ReadOperations, bool WriteOperations>
bool DrawableFilenameText<ReadOperations, WriteOperations>::IsRenaming()
{
	if (!WriteOperations)
		return false;
	return m_activeActions[RENAMING_ACTION];
}

template<bool ReadOperations, bool WriteOperations>
std::string DrawableFilenameText<ReadOperations, WriteOperations>::GetFilename()
{
	return m_filename;
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::Capture()
{
	if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
		OnControlKeyPressedEvent(CoreWindow::GetEvent().key.code);
	else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		OnControlKeyReleasedEvent(CoreWindow::GetEvent().key.code);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::Update()
{
	sf::Color color = m_statusText.getFillColor();
	m_alphaTimer.Decrement();
	color.a = static_cast<sf::Uint8>(m_alphaTimer.Value());
	m_statusText.setFillColor(color);

	sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
	m_drawableTripleText.Update();
	m_statusText.setPosition(m_statusPosition + viewOffset);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::Draw()
{
	m_drawableTripleText.Draw();
	CoreWindow::GetRenderWindow().draw(m_statusText);
}

template DrawableFilenameText<true, false>;
template DrawableFilenameText<true, true>;