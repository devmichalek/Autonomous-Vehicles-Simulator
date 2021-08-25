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
				m_texts[VARIABLE_TEXT].setString(m_filename);
			}
		}
		else
		{
			result = m_pressedControlKeys.find(eventKey);
			if (result != m_pressedControlKeys.end())
			{
				if (result->first == sf::Keyboard::BackSpace)
				{
					if (!m_filename.empty() && m_pressedBackspaceKeyTimer.Update())
					{
						m_filename.erase(m_filename.begin() + (m_filename.size() - 1));
						m_texts[VARIABLE_TEXT].setString(m_filename);
					}
				}
				else if (result->first == sf::Keyboard::LShift || result->first == sf::Keyboard::RShift)
					result->second = true;
				else if (result->first == sf::Keyboard::Escape)
				{
					if (m_filename.empty())
					{
						m_filename = m_filenameDummy;
						m_texts[VARIABLE_TEXT].setString(m_filename);
					}

					m_pressedControlKeys[sf::Keyboard::Escape] = false;
					SetVariableTextColor();
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
				SetVariableTextColor(sf::Color::Yellow);
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
DrawableFilenameText<ReadOperations, WriteOperations>::DrawableFilenameText(size_t size) :
	DrawableTripleText(size),
	m_alphaTimer(0.0, 255.0, 75.0),
	m_maxFilenameLength(18),
	m_pressedBackspaceKeyTimer(0.0, 1.0, 5000.0)
{
	std::string informationString = "| ";
	if (WriteOperations)
		informationString += "[Ctrl]+[S] ";
	if (ReadOperations)
		informationString += "[Ctrl]+[O] ";
	informationString += "[Ctrl]+[R] [Esc]";

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

	// Set strings
	SetStrings({ "Filename:", m_filename, informationString });
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
	m_texts[VARIABLE_TEXT].setString(m_filename);

	// Reset status text
	m_alphaTimer.SetTimeout();
	DrawableTripleText::Reset();
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetErrorStatusText(std::string text)
{
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(sf::Color::Red);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetSuccessStatusText(std::string text)
{
	m_texts[STATUS_TEXT].setString(text);
	m_texts[STATUS_TEXT].setFillColor(sf::Color::Green);
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::ShowStatusText()
{
	m_alphaTimer.Reset();
	DrawableTripleText::Reset();
}

template<bool ReadOperations, bool WriteOperations>
void DrawableFilenameText<ReadOperations, WriteOperations>::SetPosition(std::vector<FontContext::Component> components)
{
	ValidateNumberOfComponents(components, 5);

	float statusX = FontContext::CalculateRow(components[4]);
	float statusY = FontContext::CalculateColumn(components[0]);
	m_textPositions[STATUS_TEXT] = sf::Vector2f(statusX, statusY);
	components.pop_back();

	DrawableTripleText::SetPosition(components);
	
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
void DrawableFilenameText<ReadOperations, WriteOperations>::UpdateInternal()
{
	m_alphaTimer.Update();
	sf::Color color = m_texts[STATUS_TEXT].getFillColor();
	color.a = static_cast<sf::Uint8>(255.0 - m_alphaTimer.Value());
	m_texts[STATUS_TEXT].setFillColor(color);
}

template DrawableFilenameText<true, false>;
template DrawableFilenameText<true, true>;