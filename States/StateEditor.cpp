#include "StateEditor.hpp"
#include "CoreWindow.hpp"
#include <fstream>

void StateEditor::updateTextsPosition()
{
	sf::Vector2f viewOffset = CoreWindow::getViewOffset();

	float textX = viewOffset.x + float(CoreWindow::getSize().x) / 256;
	float h = float(CoreWindow::getSize().x) / 88;
	float textY = viewOffset.y + float(CoreWindow::getSize().y) - h;
	float activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 16;
	float helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 7.0f;
	m_wallModeText.setPosition(sf::Vector2f(textX, textY));
	m_wallModeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_wallModeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_segmentsCountText.setPosition(sf::Vector2f(textX, textY));
	m_segmentsCountActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	
	textY = viewOffset.y + float(CoreWindow::getSize().y) - h;
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 8.5f;
	m_carModeText.setPosition(sf::Vector2f(textX, textY));
	m_carModeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_carModeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_carAngleText.setPosition(sf::Vector2f(textX, textY));
	m_carAngleActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_carAngleHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY = viewOffset.y + float(CoreWindow::getSize().y) / 256;
	activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 14;
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 8;
	m_activeModeText.setPosition(sf::Vector2f(textX, textY));
	m_activeModeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_activeModeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY += h;
	m_movementText.setPosition(sf::Vector2f(textX, textY));
	m_movementActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_movementHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY += h;
	m_viewOffsetXText.setPosition(sf::Vector2f(textX, textY));
	m_viewOffsetXActiveText.setPosition(sf::Vector2f(activeTextX, textY));

	textY += h;
	m_viewOffsetYText.setPosition(sf::Vector2f(textX, textY));
	m_viewOffsetYActiveText.setPosition(sf::Vector2f(activeTextX, textY));

	textY += h;
	m_saveText.setPosition(sf::Vector2f(textX, textY));
	m_saveActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_saveHelpText.setPosition(sf::Vector2f(helpTextX, textY));
	float errorTextX = helpTextX + float(CoreWindow::getSize().x) / 10;
	m_saveErrorText.setPosition(sf::Vector2f(errorTextX, textY));
}

void StateEditor::save()
{
	updateSaveErrorText();

	if (!m_saved)
	{
		std::ofstream output("output.txt");
		if (output.is_open())
		{
			if (m_drawCar)
			{
				output << "Car angle: " << m_drawableCar.getAngle() << std::endl;
				output << "Car center: " << m_drawableCar.getCenter().x << " " << m_drawableCar.getCenter().y << std::endl;
				if (m_segments.empty())
					updateSaveErrorText(SaveError::NO_WALLS);
				else
				{
					for (auto& i : m_segments)
						output << "Segment: " << i[0].x << " " << i[0].y << " " << i[1].x << " " << i[1].y << std::endl;
					updateSaveActiveText(true);
				}
			}
			else
				updateSaveErrorText(SaveError::NO_CAR);
		}
		else
			updateSaveErrorText(SaveError::CANNOT_OPEN_FILE);
	}
}

void StateEditor::capture()
{
	if (CoreWindow::getEvent().type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2i mousePosition = CoreWindow::getMousePosition();
		sf::Vector2f viewOffset = CoreWindow::getViewOffset();
		sf::Vector2f correctPosition;
		correctPosition.x = static_cast<float>(mousePosition.x + viewOffset.x);
		correctPosition.y = static_cast<float>(mousePosition.y + viewOffset.y);

		switch (m_activeMode)
		{
			case ActiveMode::CAR:
			{
				switch (m_carMode)
				{
					case CarMode::INSERT:
					{
						m_drawCar = true;
						m_drawableCar.setCenter(correctPosition);
						m_drawableCar.update();
						updateSaveActiveText(false);
						break;
					}
					case CarMode::REMOVE:
					{
						if (m_drawableCar.intersect(correctPosition))
						{
							m_drawCar = false;
							updateSaveActiveText(false);
						}
						break;
					}
					default:
						break;
				}

				break;
			}
			case ActiveMode::WALL:
			{
				switch (m_wallMode)
				{
					case WallMode::INSERT:
					{
						if (m_insertSegment)
						{
							Segment newSegment;
							newSegment[0] = m_segmentBeggining;
							newSegment[1] = correctPosition;
							m_segments.push_back(newSegment);
							m_segmentsCountActiveText.setString(getSegmentsCountString());
							updateSaveActiveText(false);
						}
						else
							m_segmentBeggining = correctPosition;

						m_insertSegment = !m_insertSegment;
						break;
					}
					case WallMode::GLUED_INSERT:
					{
						if (m_insertSegment)
						{
							Segment newSegment;
							newSegment[0] = m_segmentBeggining;
							newSegment[1] = correctPosition;
							m_segments.push_back(newSegment);
							m_segmentsCountActiveText.setString(getSegmentsCountString());
							updateSaveActiveText(false);
						}
						else
							m_insertSegment = true;

						m_segmentBeggining = correctPosition;
						break;
					}
					case WallMode::REMOVE:
					{
						if (m_removeSegment)
						{
							size_t size = m_segments.size();
							for (size_t i = 0; i < size; ++i)
							{
								if (::intersect(m_segmentBeggining, correctPosition, m_segments[i][0], m_segments[i][1]))
								{
									updateSaveActiveText(false);
									m_segments.erase(m_segments.begin() + i);
									--size;
									--i;
								}
							}
							m_segmentsCountActiveText.setString(getSegmentsCountString());
						}
						else
							m_segmentBeggining = correctPosition;

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
				m_carMode = CarMode::INSERT;
				m_activeMode = ActiveMode::WALL;
				m_activeModeActiveText.setString(getActiveModeString());
			}
			else
			{
				CarMode mode = m_carMode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = CarMode::INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
						 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
				{
					mode = CarMode::REMOVE;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				{
					m_drawableCar.rotate(-1);
					m_drawableCar.update();
					m_carAngleActiveText.setString(getCarAngleString());
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				{
					m_drawableCar.rotate(1);
					m_drawableCar.update();
					m_carAngleActiveText.setString(getCarAngleString());
				}

				if (m_carMode != mode)
				{
					m_carMode = mode;
					m_carModeActiveText.setString(getCarModeString());
				}
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
			else
			{
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
			}

			break;
		}
		default:
			break;
	}

	float elapsedTime = float(CoreWindow::getElapsedTime());
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
		m_viewOffsetXActiveText.setString(getViewOffsetXString());
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(m_movement * elapsedTime, 0));
		CoreWindow::getRenderWindow().setView(view);
		m_viewOffsetXActiveText.setString(getViewOffsetXString());
		updateTextsPosition();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, -m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		m_viewOffsetYActiveText.setString(getViewOffsetYString());
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		m_viewOffsetYActiveText.setString(getViewOffsetYString());
		updateTextsPosition();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			if (!m_saveKeysPressed)
			{
				m_saveKeysPressed = true;
				save();
			}
		}
		else
			m_saveKeysPressed = false;
	}
	else
		m_saveKeysPressed = false;
}

void StateEditor::load()
{
	if (m_font.loadFromFile("consola.ttf"))
	{
		m_wallModeText.setFont(m_font);
		m_wallModeActiveText.setFont(m_font);
		m_wallModeHelpText.setFont(m_font);
		m_carModeText.setFont(m_font);
		m_carModeActiveText.setFont(m_font);
		m_carModeHelpText.setFont(m_font);
		m_segmentsCountText.setFont(m_font);
		m_segmentsCountActiveText.setFont(m_font);
		m_carAngleText.setFont(m_font);
		m_carAngleActiveText.setFont(m_font);
		m_carAngleHelpText.setFont(m_font);
		m_activeModeText.setFont(m_font);
		m_activeModeActiveText.setFont(m_font);
		m_activeModeHelpText.setFont(m_font);
		m_movementText.setFont(m_font);
		m_movementActiveText.setFont(m_font);
		m_movementHelpText.setFont(m_font);
		m_viewOffsetXText.setFont(m_font);
		m_viewOffsetXActiveText.setFont(m_font);
		m_viewOffsetYText.setFont(m_font);
		m_viewOffsetYActiveText.setFont(m_font);
		m_saveText.setFont(m_font);
		m_saveActiveText.setFont(m_font);
		m_saveHelpText.setFont(m_font);
		m_saveErrorText.setFont(m_font);

		auto activeColor = sf::Color(0xC0, 0xC0, 0xC0, 0xFF);
		m_wallModeActiveText.setFillColor(activeColor);
		m_carModeActiveText.setFillColor(activeColor);
		m_segmentsCountActiveText.setFillColor(activeColor);
		m_carAngleActiveText.setFillColor(activeColor);
		m_activeModeActiveText.setFillColor(activeColor);
		m_movementActiveText.setFillColor(activeColor);
		m_viewOffsetXActiveText.setFillColor(activeColor);
		m_viewOffsetYActiveText.setFillColor(activeColor);
		m_saveActiveText.setFillColor(sf::Color::Red);

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_wallModeText.setCharacterSize(characterSize);
		m_wallModeActiveText.setCharacterSize(characterSize);
		m_wallModeHelpText.setCharacterSize(characterSize);
		m_carModeText.setCharacterSize(characterSize);
		m_carModeActiveText.setCharacterSize(characterSize);
		m_carModeHelpText.setCharacterSize(characterSize);
		m_segmentsCountText.setCharacterSize(characterSize);
		m_segmentsCountActiveText.setCharacterSize(characterSize);
		m_carAngleText.setCharacterSize(characterSize);
		m_carAngleActiveText.setCharacterSize(characterSize);
		m_carAngleHelpText.setCharacterSize(characterSize);
		m_activeModeText.setCharacterSize(characterSize);
		m_activeModeActiveText.setCharacterSize(characterSize);
		m_activeModeHelpText.setCharacterSize(characterSize);
		m_movementText.setCharacterSize(characterSize);
		m_movementActiveText.setCharacterSize(characterSize);
		m_movementHelpText.setCharacterSize(characterSize);
		m_viewOffsetXText.setCharacterSize(characterSize);
		m_viewOffsetXActiveText.setCharacterSize(characterSize);
		m_viewOffsetYText.setCharacterSize(characterSize);
		m_viewOffsetYActiveText.setCharacterSize(characterSize);
		m_saveText.setCharacterSize(characterSize);
		m_saveActiveText.setCharacterSize(characterSize);
		m_saveHelpText.setCharacterSize(characterSize);
		m_saveErrorText.setCharacterSize(characterSize);

		m_wallModeText.setString("Current mode:");
		m_wallModeActiveText.setString(getWallModeString());
		m_wallModeHelpText.setString("| Keys: [1] [2] [3]");
		m_carModeText.setString("Current mode:");
		m_carModeActiveText.setString(getCarModeString());
		m_carModeHelpText.setString("| Keys: [1] [2]");
		m_segmentsCountText.setString("Wall count:");
		m_segmentsCountActiveText.setString(getSegmentsCountString());
		m_carAngleText.setString("Car angle:");
		m_carAngleActiveText.setString(getCarAngleString());
		m_carAngleHelpText.setString("| Keys: [Z] [X]");
		m_activeModeText.setString("Active mode:");
		m_activeModeActiveText.setString(getActiveModeString());
		m_activeModeHelpText.setString("| Keys: [F1] [F2]");
		m_movementText.setString("Movement:");
		m_movementActiveText.setString(getMovementString());
		m_movementHelpText.setString("| Keys: [+] [-]");
		m_viewOffsetXText.setString("View offset x:");
		m_viewOffsetXActiveText.setString(getViewOffsetXString());
		m_viewOffsetYText.setString("View offset y:");
		m_viewOffsetYActiveText.setString(getViewOffsetYString());
		m_saveText.setString("Save status:");
		m_saveHelpText.setString("| Keys: [Ctrl] + [S]");

		updateSaveActiveText(false);
		updateSaveErrorText();
		updateTextsPosition();
	}
}

void StateEditor::draw()
{
	if (m_drawCar)
		m_drawableCar.draw();

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
			CoreWindow::getRenderWindow().draw(m_carModeText);
			CoreWindow::getRenderWindow().draw(m_carModeActiveText);
			CoreWindow::getRenderWindow().draw(m_carModeHelpText);
			CoreWindow::getRenderWindow().draw(m_carAngleText);
			CoreWindow::getRenderWindow().draw(m_carAngleActiveText);
			CoreWindow::getRenderWindow().draw(m_carAngleHelpText);
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
				m_line[1].position += CoreWindow::getViewOffset();
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
				m_line[1].position += CoreWindow::getViewOffset();
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
	CoreWindow::getRenderWindow().draw(m_viewOffsetXText);
	CoreWindow::getRenderWindow().draw(m_viewOffsetXActiveText);
	CoreWindow::getRenderWindow().draw(m_viewOffsetYText);
	CoreWindow::getRenderWindow().draw(m_viewOffsetYActiveText);
	CoreWindow::getRenderWindow().draw(m_saveText);
	CoreWindow::getRenderWindow().draw(m_saveActiveText);
	CoreWindow::getRenderWindow().draw(m_saveHelpText);
	CoreWindow::getRenderWindow().draw(m_saveErrorText);
}