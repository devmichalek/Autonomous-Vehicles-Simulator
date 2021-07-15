#include "DrawableWallEditor.hpp"

void DrawableWallEditor::capture()
{
	if (CoreWindow::getEvent().type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2i mousePosition = CoreWindow::getMousePosition();
		switch (m_currentMode)
		{
			case Mode::INSERT:
			{
				if (m_insertSegment)
				{
					Segment newSegment;
					newSegment[0] = m_segmentBeggining;
					newSegment[1].x = static_cast<float>(mousePosition.x);
					newSegment[1].y = static_cast<float>(mousePosition.y);
					m_segments.push_back(newSegment);
					m_segmentsCountActiveText.setString(getSegmentsCount());
				}
				else
				{
					m_segmentBeggining.x = static_cast<float>(mousePosition.x);
					m_segmentBeggining.y = static_cast<float>(mousePosition.y);
				}

				m_insertSegment = !m_insertSegment;
				break;
			}
			case Mode::GLUED_INSERT:
			{
				if (m_insertSegment)
				{
					Segment newSegment;
					newSegment[0] = m_segmentBeggining;
					newSegment[1].x = static_cast<float>(mousePosition.x);
					newSegment[1].y = static_cast<float>(mousePosition.y);
					m_segments.push_back(newSegment);
					m_segmentsCountActiveText.setString(getSegmentsCount());
				}
				else
					m_insertSegment = true;

				m_segmentBeggining.x = static_cast<float>(mousePosition.x);
				m_segmentBeggining.y = static_cast<float>(mousePosition.y);
				break;
			}
			case Mode::REMOVE:
			{
				if (m_removeSegment)
				{
					sf::Vector2f position;
					position.x = static_cast<float>(mousePosition.x);
					position.y = static_cast<float>(mousePosition.y);
					size_t size = m_segments.size();
					for (size_t i = 0; i < size; ++i)
					{
						if (::intersect(m_segmentBeggining, position, m_segments[i][0], m_segments[i][1]))
						{
							m_segments.erase(m_segments.begin() + i);
							--size;
							--i;
						}
					}
					m_segmentsCountActiveText.setString(getSegmentsCount());
				}
				else
				{
					m_segmentBeggining.x = static_cast<float>(mousePosition.x);
					m_segmentBeggining.y = static_cast<float>(mousePosition.y);
				}

				m_removeSegment = !m_removeSegment;
				break;
			}
		}
	}
}

void DrawableWallEditor::update()
{
	Mode mode = m_currentMode;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
	{
		mode = Mode::INSERT;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
	{
		mode = Mode::GLUED_INSERT;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
	{
		mode = Mode::REMOVE;
	}

	if (m_currentMode != mode)
	{
		m_currentMode = mode;
		m_insertSegment = false;
		m_removeSegment = false;
		m_currentModeActiveText.setString(getCurrentMode());
	}
}

void DrawableWallEditor::load()
{
	if (m_font.loadFromFile("consola.ttf"))
	{
		m_currentModeText.setFont(m_font);
		m_currentModeActiveText.setFont(m_font);
		m_segmentsCountText.setFont(m_font);
		m_segmentsCountActiveText.setFont(m_font);

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_currentModeText.setCharacterSize(characterSize);
		m_currentModeActiveText.setCharacterSize(characterSize);
		m_segmentsCountText.setCharacterSize(characterSize);
		m_segmentsCountActiveText.setCharacterSize(characterSize);

		m_currentModeText.setString("Current mode:");
		m_currentModeActiveText.setString(getCurrentMode());
		m_segmentsCountText.setString("Segments count:");
		m_segmentsCountActiveText.setString(getSegmentsCount());

		float h = (CoreWindow::getSize().x / 88);
		float y = CoreWindow::getSize().y - h;
		m_currentModeText.setPosition(sf::Vector2f(float(CoreWindow::getSize().x) / 256, y));
		m_currentModeActiveText.setPosition(sf::Vector2f(float(CoreWindow::getSize().x) / 16, y));
		m_segmentsCountText.setPosition(sf::Vector2f(float(CoreWindow::getSize().x) / 256, y - h));
		m_segmentsCountActiveText.setPosition(sf::Vector2f(float(CoreWindow::getSize().x) / 14, y - h));
	}
}

void DrawableWallEditor::draw()
{
	m_line[0].color = sf::Color::White;
	m_line[1].color = sf::Color::White;
	for (const auto& i : m_segments)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	if (m_active)
	{
		if (m_insertSegment)
		{
			sf::Vector2i mousePosition = CoreWindow::getMousePosition();
			m_line[0].position = m_segmentBeggining;
			m_line[1].position.x = static_cast<float>(mousePosition.x);
			m_line[1].position.y = static_cast<float>(mousePosition.y);
			CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		}

		if (m_removeSegment)
		{
			sf::Vector2i mousePosition = CoreWindow::getMousePosition();
			m_line[0].position = m_segmentBeggining;
			m_line[1].position.x = static_cast<float>(mousePosition.x);
			m_line[1].position.y = static_cast<float>(mousePosition.y);
			m_line[0].color = sf::Color::Red;
			m_line[1].color = sf::Color::Red;
			CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
		}

		CoreWindow::getRenderWindow().draw(m_currentModeText);
		CoreWindow::getRenderWindow().draw(m_currentModeActiveText);
		CoreWindow::getRenderWindow().draw(m_segmentsCountText);
		CoreWindow::getRenderWindow().draw(m_segmentsCountActiveText);
	}
}