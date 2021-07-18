#include "StateEditor.hpp"
#include "CoreWindow.hpp"

void StateEditor::updateTextsPosition()
{
	auto viewOffset = CoreWindow::getViewOffset();

	float textX = viewOffset.x + float(CoreWindow::getSize().x) / 256;
	float h = float(CoreWindow::getSize().x) / 88;
	float textY = viewOffset.y + float(CoreWindow::getSize().y) - h;
	float activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 14;
	float helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 7;
	m_wallModeText.setPosition(sf::Vector2f(textX, textY));
	m_wallModeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_wallModeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_segmentsCountText.setPosition(sf::Vector2f(textX, textY));
	m_segmentsCountActiveText.setPosition(sf::Vector2f(activeTextX, textY));

	textY = viewOffset.y + float(CoreWindow::getSize().y) / 256;
	activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 17;
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 10;
	m_activeModeText.setPosition(sf::Vector2f(textX, textY));
	m_activeModeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_activeModeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY += h;
	m_movementText.setPosition(sf::Vector2f(textX, textY));
	m_movementActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_movementHelpText.setPosition(sf::Vector2f(helpTextX, textY));
}

void StateEditor::capture()
{
	if (CoreWindow::getEvent().type == sf::Event::MouseButtonPressed)
	{
		switch (m_activeMode)
		{
			case ActiveMode::CAR:
			{
				

				break;
			}
			case ActiveMode::WALL:
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				switch (m_wallMode)
				{
					case WallMode::INSERT:
					{
						if (m_insertSegment)
						{
							Segment newSegment;
							newSegment[0] = m_segmentBeggining;
							newSegment[1].x = static_cast<float>(mousePosition.x);
							newSegment[1].y = static_cast<float>(mousePosition.y);
							m_segments.push_back(newSegment);
							m_segmentsCountActiveText.setString(getSegmentsCountString());
						}
						else
						{
							m_segmentBeggining.x = static_cast<float>(mousePosition.x);
							m_segmentBeggining.y = static_cast<float>(mousePosition.y);
						}

						m_insertSegment = !m_insertSegment;
						break;
					}
					case WallMode::GLUED_INSERT:
					{
						if (m_insertSegment)
						{
							Segment newSegment;
							newSegment[0] = m_segmentBeggining;
							newSegment[1].x = static_cast<float>(mousePosition.x);
							newSegment[1].y = static_cast<float>(mousePosition.y);
							m_segments.push_back(newSegment);
							m_segmentsCountActiveText.setString(getSegmentsCountString());
						}
						else
							m_insertSegment = true;

						m_segmentBeggining.x = static_cast<float>(mousePosition.x);
						m_segmentBeggining.y = static_cast<float>(mousePosition.y);
						break;
					}
					case WallMode::REMOVE:
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
							m_segmentsCountActiveText.setString(getSegmentsCountString());
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
				break;
			}
		}
	}
}

void StateEditor::update()
{
	switch (m_activeMode)
	{
		case ActiveMode::CAR:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
			{
				m_activeMode = ActiveMode::WALL;
				m_activeModeActiveText.setString(getActiveModeString());
			}

			break;
		}
		case ActiveMode::WALL:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
			{
				m_wallMode = WallMode::INSERT;
				m_insertSegment = false;
				m_removeSegment = false;
				m_wallModeActiveText.setString(getWallModeString());
				m_activeMode = ActiveMode::CAR;
				m_activeModeActiveText.setString(getActiveModeString());
			}

			WallMode mode = m_wallMode;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
			{
				mode = WallMode::INSERT;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
			{
				mode = WallMode::GLUED_INSERT;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
			{
				mode = WallMode::REMOVE;
			}

			if (m_wallMode != mode)
			{
				m_wallMode = mode;
				m_insertSegment = false;
				m_removeSegment = false;
				m_wallModeActiveText.setString(getWallModeString());
			}
			break;
		}
		default:
			break;
	}

	double elapsedTime = CoreWindow::getElapsedTime();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
	{
		m_movement += elapsedTime * m_movementConst;
		if (m_movement > m_movementMax)
			m_movement = m_movementMax;
		m_movementActiveText.setString(getMovementString());
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
	{
		m_movement -= elapsedTime * m_movementConst;
		if (m_movement < m_movementMin)
			m_movement = m_movementMin;
		m_movementActiveText.setString(getMovementString());
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(-m_movement * elapsedTime, 0));
		CoreWindow::getRenderWindow().setView(view);
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(m_movement * elapsedTime, 0));
		CoreWindow::getRenderWindow().setView(view);
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, -m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		updateTextsPosition();
	}
}

void StateEditor::load()
{
	if (m_font.loadFromFile("consola.ttf"))
	{
		m_wallModeText.setFont(m_font);
		m_wallModeActiveText.setFont(m_font);
		m_wallModeHelpText.setFont(m_font);
		m_segmentsCountText.setFont(m_font);
		m_segmentsCountActiveText.setFont(m_font);
		m_activeModeText.setFont(m_font);
		m_activeModeActiveText.setFont(m_font);
		m_activeModeHelpText.setFont(m_font);
		m_movementText.setFont(m_font);
		m_movementActiveText.setFont(m_font);
		m_movementHelpText.setFont(m_font);

		m_wallModeActiveText.setFillColor(sf::Color(0xC0, 0xC0, 0xC0, 0xFF));
		m_segmentsCountActiveText.setFillColor(sf::Color(0xC0, 0xC0, 0xC0, 0xFF));
		m_activeModeActiveText.setFillColor(sf::Color(0xC0, 0xC0, 0xC0, 0xFF));
		m_movementActiveText.setFillColor(sf::Color(0xC0, 0xC0, 0xC0, 0xFF));

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_wallModeText.setCharacterSize(characterSize);
		m_wallModeActiveText.setCharacterSize(characterSize);
		m_wallModeHelpText.setCharacterSize(characterSize);
		m_segmentsCountText.setCharacterSize(characterSize);
		m_segmentsCountActiveText.setCharacterSize(characterSize);
		m_activeModeText.setCharacterSize(characterSize);
		m_activeModeActiveText.setCharacterSize(characterSize);
		m_activeModeHelpText.setCharacterSize(characterSize);
		m_movementText.setCharacterSize(characterSize);
		m_movementActiveText.setCharacterSize(characterSize);
		m_movementHelpText.setCharacterSize(characterSize);

		m_wallModeText.setString("Current mode:");
		m_wallModeActiveText.setString(getWallModeString());
		m_wallModeHelpText.setString("| Keys: [1] [2] [3]");
		m_segmentsCountText.setString("Segments count:");
		m_segmentsCountActiveText.setString(getSegmentsCountString());
		m_activeModeText.setString("Active mode:");
		m_activeModeActiveText.setString(getActiveModeString());
		m_activeModeHelpText.setString("| Keys: [F1] [F2]");
		m_movementText.setString("Movement:");
		m_movementActiveText.setString(getMovementString());
		m_movementHelpText.setString("| Keys: [+] [-]");

		updateTextsPosition();
	}
}

void StateEditor::draw()
{
	m_line[0].color = sf::Color::White;
	m_line[1].color = sf::Color::White;
	for (const auto& i : m_segments)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	switch (m_activeMode)
	{
		case ActiveMode::CAR:
		{

			break;
		}
		case ActiveMode::WALL:
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

			CoreWindow::getRenderWindow().draw(m_wallModeText);
			CoreWindow::getRenderWindow().draw(m_wallModeActiveText);
			CoreWindow::getRenderWindow().draw(m_wallModeHelpText);
			CoreWindow::getRenderWindow().draw(m_segmentsCountText);
			CoreWindow::getRenderWindow().draw(m_segmentsCountActiveText);
			break;
		}
		default:
			break;
	}

	CoreWindow::getRenderWindow().draw(m_activeModeText);
	CoreWindow::getRenderWindow().draw(m_activeModeActiveText);
	CoreWindow::getRenderWindow().draw(m_activeModeHelpText);
	CoreWindow::getRenderWindow().draw(m_movementText);
	CoreWindow::getRenderWindow().draw(m_movementActiveText);
	CoreWindow::getRenderWindow().draw(m_movementHelpText);
}