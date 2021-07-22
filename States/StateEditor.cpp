#include "StateEditor.hpp"
#include "CoreWindow.hpp"
#include "DrawableBuilder.hpp"

void StateEditor::setWallCountActiveText()
{
	m_wallCountActiveText.setString(std::to_string(m_walls.size()));
}

void StateEditor::setCarAngleActiveText()
{
	long long angle = static_cast<long long>(m_drawableCar.getAngle());
	angle %= 360;
	if (angle < 0)
		angle += 360;
	m_carAngleActiveText.setString(std::to_string(angle));
}

void StateEditor::setMovementActiveText()
{
	m_movementActiveText.setString(std::to_string(long(m_movement)));
}

void StateEditor::setViewOffsetXActiveText()
{
	sf::Vector2f viewOffset = CoreWindow::getViewOffset();
	m_viewOffsetXActiveText.setString(std::to_string(int(viewOffset.x)));
}

void StateEditor::setViewOffsetYActiveText()
{
	sf::Vector2f viewOffset = CoreWindow::getViewOffset();
	m_viewOffsetYActiveText.setString(std::to_string(int(viewOffset.y)));
}

void StateEditor::setUpToDate()
{
	m_saveStatus = SaveStatus::UP_TO_DATE;
	m_saveActiveText.setString(std::get<1>(m_saveStatusMap[m_saveStatus]));
}

void StateEditor::setOutOfDate()
{
	m_saveStatus = SaveStatus::OUT_OF_DATE;
	m_saveActiveText.setString(std::get<1>(m_saveStatusMap[m_saveStatus]));
}

void StateEditor::updateTextsPosition()
{
	sf::Vector2f viewOffset = CoreWindow::getViewOffset();

	float textX = viewOffset.x + float(CoreWindow::getSize().x) / 256;
	float h = float(CoreWindow::getSize().x) / 88;
	float textY = viewOffset.y + float(CoreWindow::getSize().y) - h;
	float activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 16;
	float helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 7.0f;
	m_wallSubmodeText.setPosition(sf::Vector2f(textX, textY));
	m_wallSubmodeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_wallSubmodeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_wallCountText.setPosition(sf::Vector2f(textX, textY));
	m_wallCountActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	
	textY = viewOffset.y + float(CoreWindow::getSize().y) - h;
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 8.5f;
	m_carSubmodeText.setPosition(sf::Vector2f(textX, textY));
	m_carSubmodeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_carSubmodeHelpText.setPosition(sf::Vector2f(helpTextX, textY));
	m_finishLineSubmodeText.setPosition(sf::Vector2f(textX, textY));
	m_finishLineSubmodeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_finishLineSubmodeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_carAngleText.setPosition(sf::Vector2f(textX, textY));
	m_carAngleActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_carAngleHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY = viewOffset.y + float(CoreWindow::getSize().y) / 256;
	activeTextX = viewOffset.x + float(CoreWindow::getSize().x) / 14;
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 7;
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
	m_saveStatusText.setPosition(sf::Vector2f(errorTextX, textY));
}

void StateEditor::save()
{
	SaveStatus result = SaveStatus::OUT_OF_DATE;
	switch (m_saveStatus)
	{
		case SaveStatus::UP_TO_DATE:
			result = SaveStatus::UP_TO_DATE;
			break;
		default:
		{
			if (m_walls.empty())
			{
				result = SaveStatus::ERROR_NO_WALLS_POSITIONED;
				break;
			}

			if (!m_drawCar)
			{
				result = SaveStatus::ERROR_NO_CAR_POSITIONED;
				break;
			}

			if (!m_drawFinishLine)
			{
				result = SaveStatus::ERROR_NO_FINISH_LINE_POSITIONED;
				break;
			}

			DrawableBuilder builder;
			builder.addCar(m_drawableCar.getAngle(), m_drawableCar.getCenter());
			for (auto& i : m_walls)
				builder.addWall(i);
			Segment segment = { m_drawableFinishLine.getStartPoint(), m_drawableFinishLine.getEndPoint() };
			builder.addFinishLine(segment);

			if (!builder.save())
			{
				result = SaveStatus::ERROR_CANNOT_OPEN_FILE;
				break;
			}

			result = SaveStatus::UP_TO_DATE;
			break;
		}
	}

	m_saveStatus = result;
	m_saveStatusAlpha = m_saveStatusAlphaMax;
	m_saveActiveText.setString(std::get<1>(m_saveStatusMap[m_saveStatus]));
	m_saveStatusText.setString(std::get<0>(m_saveStatusMap[m_saveStatus]));
	m_saveStatusText.setFillColor(std::get<2>(m_saveStatusMap[m_saveStatus]));
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
			case ActiveMode::WALL:
			{
				switch (m_wallSubmode)
				{
					case WallSubmode::INSERT:
					{
						if (m_insertWall)
						{
							Segment newSegment;
							newSegment[0] = m_wallBeggining;
							newSegment[1] = correctPosition;
							m_walls.push_back(newSegment);
							setWallCountActiveText();
							setOutOfDate();
						}
						else
							m_wallBeggining = correctPosition;

						m_insertWall = !m_insertWall;
						break;
					}
					case WallSubmode::GLUED_INSERT:
					{
						if (m_insertWall)
						{
							Segment newSegment;
							newSegment[0] = m_wallBeggining;
							newSegment[1] = correctPosition;
							m_walls.push_back(newSegment);
							setWallCountActiveText();
							setOutOfDate();
						}
						else
							m_insertWall = true;

						m_wallBeggining = correctPosition;
						break;
					}
					case WallSubmode::REMOVE:
					{
						if (m_removeWall)
						{
							size_t size = m_walls.size();
							for (size_t i = 0; i < size; ++i)
							{
								if (intersect(m_wallBeggining, correctPosition, m_walls[i][0], m_walls[i][1]))
								{
									setOutOfDate();
									m_walls.erase(m_walls.begin() + i);
									--size;
									--i;
								}
							}
							setWallCountActiveText();
						}
						else
							m_wallBeggining = correctPosition;

						m_removeWall = !m_removeWall;
						break;
					}
				}
				break;
			}
			case ActiveMode::CAR:
			{
				switch (m_carSubmode)
				{
					case CarSubmode::INSERT:
					{
						m_drawCar = true;
						m_drawableCar.setCenter(correctPosition);
						m_drawableCar.update();
						setOutOfDate();
						break;
					}
					case CarSubmode::REMOVE:
					{
						if (m_drawableCar.intersect(correctPosition))
						{
							m_drawCar = false;
							setOutOfDate();
						}
						break;
					}
					default:
						break;
				}

				break;
			}
			case ActiveMode::FINISH_LINE:
			{
				switch (m_finishLineSubmode)
				{
					case FinishLineSubmode::INSERT:
					{
						if (m_insertFinishLine)
						{
							m_drawableFinishLine.setStartPoint(m_finishLineBeggining);
							m_drawableFinishLine.setEndPoint(correctPosition);
							m_drawFinishLine = true;
							setOutOfDate();
						}
						else
						{
							m_drawFinishLine = false;
							m_finishLineBeggining = correctPosition;
						}
					
						m_insertFinishLine = !m_insertFinishLine;
						break;
					}
					default:
						break;
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
		case ActiveMode::WALL:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
			{
				m_wallSubmode = WallSubmode::INSERT;
				m_insertWall = false;
				m_removeWall = false;
				m_wallSubmodeActiveText.setString(m_wallSubmodeMap[m_wallSubmode]);
				m_activeMode = ActiveMode::CAR;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
			{
				m_wallSubmode = WallSubmode::INSERT;
				m_insertWall = false;
				m_removeWall = false;
				m_wallSubmodeActiveText.setString(m_wallSubmodeMap[m_wallSubmode]);
				m_activeMode = ActiveMode::FINISH_LINE;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else
			{
				WallSubmode mode = m_wallSubmode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = WallSubmode::INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
						 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
				{
					mode = WallSubmode::GLUED_INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) ||
						 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3))
				{
					mode = WallSubmode::REMOVE;
				}

				if (m_wallSubmode != mode)
				{
					m_wallSubmode = mode;
					m_insertWall = false;
					m_removeWall = false;
					m_wallSubmodeActiveText.setString(m_wallSubmodeMap[m_wallSubmode]);
				}
			}

			break;
		}
		case ActiveMode::CAR:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
			{
				m_carSubmode = CarSubmode::INSERT;
				m_activeMode = ActiveMode::WALL;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
			{
				m_carSubmode = CarSubmode::INSERT;
				m_activeMode = ActiveMode::FINISH_LINE;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else
			{
				CarSubmode mode = m_carSubmode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = CarSubmode::INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
				{
					mode = CarSubmode::REMOVE;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				{
					m_drawableCar.rotate(-1);
					m_drawableCar.update();
					setCarAngleActiveText();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				{
					m_drawableCar.rotate(1);
					m_drawableCar.update();
					setCarAngleActiveText();
				}

				if (m_carSubmode != mode)
				{
					m_carSubmode = mode;
					m_carSubmodeActiveText.setString(m_carSubmodeMap[m_carSubmode]);
				}
			}

			break;
		}
		case ActiveMode::FINISH_LINE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
			{
				m_finishLineSubmode = FinishLineSubmode::INSERT;
				m_activeMode = ActiveMode::WALL;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
			{
				m_finishLineSubmode = FinishLineSubmode::INSERT;
				m_activeMode = ActiveMode::CAR;
				m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
			}
			else
			{
				FinishLineSubmode mode = m_finishLineSubmode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = FinishLineSubmode::INSERT;
				}

				if (m_finishLineSubmode != mode)
				{
					m_finishLineSubmode = mode;
					m_finishLineSubmodeActiveText.setString(m_finishLineSubmodeMap[m_finishLineSubmode]);
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
		setMovementActiveText();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
	{
		m_movement -= elapsedTime * m_movementConst;
		if (m_movement < m_movementMin)
			m_movement = m_movementMin;
		setMovementActiveText();
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(-m_movement * elapsedTime, 0));
		CoreWindow::getRenderWindow().setView(view);
		setViewOffsetXActiveText();
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(m_movement * elapsedTime, 0));
		CoreWindow::getRenderWindow().setView(view);
		setViewOffsetXActiveText();
		updateTextsPosition();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, -m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		setViewOffsetYActiveText();
		updateTextsPosition();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, m_movement * elapsedTime));
		CoreWindow::getRenderWindow().setView(view);
		setViewOffsetYActiveText();
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

	sf::Color color = m_saveStatusText.getFillColor();
	m_saveStatusAlpha -= m_saveStatusAlphaConst * elapsedTime;
	if (m_saveStatusAlpha < m_saveStatusAlphaMin)
		m_saveStatusAlpha = m_saveStatusAlphaMin;
	color.a = static_cast<sf::Uint8>(m_saveStatusAlpha);
	m_saveStatusText.setFillColor(color);
}

void StateEditor::load()
{
	m_activeModeMap[ActiveMode::WALL] = "Wall mode";
	m_activeModeMap[ActiveMode::CAR] = "Car mode";
	m_activeModeMap[ActiveMode::FINISH_LINE] = "Finish line mode";

	m_wallSubmodeMap[WallSubmode::INSERT] = "Insert mode";
	m_wallSubmodeMap[WallSubmode::GLUED_INSERT] = "Glued insert mode";
	m_wallSubmodeMap[WallSubmode::REMOVE] = "Remove mode";

	m_carSubmodeMap[CarSubmode::INSERT] = "Insert mode";
	m_carSubmodeMap[CarSubmode::REMOVE] = "Remove mode";

	m_finishLineSubmodeMap[FinishLineSubmode::INSERT] = "Insert mode";

	m_saveStatusMap[SaveStatus::UP_TO_DATE] = std::tuple("Success: Changes were saved", "Up to date", sf::Color::Green);
	m_saveStatusMap[SaveStatus::OUT_OF_DATE] = std::tuple("Warning: Changes not saved", "Out of date", sf::Color(0, 0, 0, 0));
	m_saveStatusMap[SaveStatus::ERROR_NO_WALLS_POSITIONED] = std::tuple("Error: No walls positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_CAR_POSITIONED] = std::tuple("Error: Car is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_FINISH_LINE_POSITIONED] = std::tuple("Error: Finish line is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_CANNOT_OPEN_FILE] = std::tuple("Error: Cannot open file!", "Out of date", sf::Color::Red);

	if (m_font.loadFromFile("consola.ttf"))
	{
		m_wallSubmodeText.setFont(m_font);
		m_wallSubmodeActiveText.setFont(m_font);
		m_wallSubmodeHelpText.setFont(m_font);
		m_carSubmodeText.setFont(m_font);
		m_carSubmodeActiveText.setFont(m_font);
		m_carSubmodeHelpText.setFont(m_font);
		m_finishLineSubmodeText.setFont(m_font);
		m_finishLineSubmodeActiveText.setFont(m_font);
		m_finishLineSubmodeHelpText.setFont(m_font);
		m_wallCountText.setFont(m_font);
		m_wallCountActiveText.setFont(m_font);
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
		m_saveStatusText.setFont(m_font);

		auto activeColor = sf::Color(0xC0, 0xC0, 0xC0, 0xFF);
		m_wallSubmodeActiveText.setFillColor(activeColor);
		m_carSubmodeActiveText.setFillColor(activeColor);
		m_finishLineSubmodeActiveText.setFillColor(activeColor);
		m_wallCountActiveText.setFillColor(activeColor);
		m_carAngleActiveText.setFillColor(activeColor);
		m_activeModeActiveText.setFillColor(activeColor);
		m_movementActiveText.setFillColor(activeColor);
		m_viewOffsetXActiveText.setFillColor(activeColor);
		m_viewOffsetYActiveText.setFillColor(activeColor);
		m_saveActiveText.setFillColor(activeColor);

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_wallSubmodeText.setCharacterSize(characterSize);
		m_wallSubmodeActiveText.setCharacterSize(characterSize);
		m_wallSubmodeHelpText.setCharacterSize(characterSize);
		m_carSubmodeText.setCharacterSize(characterSize);
		m_carSubmodeActiveText.setCharacterSize(characterSize);
		m_carSubmodeHelpText.setCharacterSize(characterSize);
		m_finishLineSubmodeText.setCharacterSize(characterSize);
		m_finishLineSubmodeActiveText.setCharacterSize(characterSize);
		m_finishLineSubmodeHelpText.setCharacterSize(characterSize);
		m_wallCountText.setCharacterSize(characterSize);
		m_wallCountActiveText.setCharacterSize(characterSize);
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
		m_saveStatusText.setCharacterSize(characterSize);

		m_wallSubmodeText.setString("Current mode:");
		m_wallSubmodeActiveText.setString(m_wallSubmodeMap[m_wallSubmode]);
		m_wallSubmodeHelpText.setString("| Keys: [1] [2] [3]");
		m_carSubmodeText.setString("Current mode:");
		m_carSubmodeActiveText.setString(m_carSubmodeMap[m_carSubmode]);
		m_carSubmodeHelpText.setString("| Keys: [1] [2]");
		m_finishLineSubmodeText.setString("Current mode:");
		m_finishLineSubmodeActiveText.setString(m_finishLineSubmodeMap[m_finishLineSubmode]);
		m_finishLineSubmodeHelpText.setString("| Keys: [1]");
		m_wallCountText.setString("Wall count:");
		setWallCountActiveText();
		m_carAngleText.setString("Car angle:");
		setCarAngleActiveText();
		m_carAngleHelpText.setString("| Keys: [Z] [X]");
		m_activeModeText.setString("Active mode:");
		m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
		m_activeModeHelpText.setString("| Keys: [F1] [F2] [F3]");
		m_movementText.setString("Movement:");
		setMovementActiveText();
		m_movementHelpText.setString("| Keys: [+] [-]");
		m_viewOffsetXText.setString("View offset x:");
		setViewOffsetXActiveText();
		m_viewOffsetYText.setString("View offset y:");
		setViewOffsetYActiveText();
		m_saveText.setString("Save status:");
		m_saveHelpText.setString("| Keys: [Ctrl] + [S]");

		updateTextsPosition();
	}

	setOutOfDate();
}

void StateEditor::draw()
{
	if (m_drawCar)
		m_drawableCar.draw();

	if (m_drawFinishLine)
		m_drawableFinishLine.draw();

	m_line[0].color = sf::Color::White;
	m_line[1].color = sf::Color::White;
	for (const auto& i : m_walls)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	switch (m_activeMode)
	{
		case ActiveMode::WALL:
		{
			if (m_insertWall)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_wallBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}
			else if (m_removeWall)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_wallBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				m_line[0].color = sf::Color::Red;
				m_line[1].color = sf::Color::Red;
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}

			CoreWindow::getRenderWindow().draw(m_wallSubmodeText);
			CoreWindow::getRenderWindow().draw(m_wallSubmodeActiveText);
			CoreWindow::getRenderWindow().draw(m_wallSubmodeHelpText);
			CoreWindow::getRenderWindow().draw(m_wallCountText);
			CoreWindow::getRenderWindow().draw(m_wallCountActiveText);
			break;
		}
		case ActiveMode::CAR:
		{
			CoreWindow::getRenderWindow().draw(m_carSubmodeText);
			CoreWindow::getRenderWindow().draw(m_carSubmodeActiveText);
			CoreWindow::getRenderWindow().draw(m_carSubmodeHelpText);
			CoreWindow::getRenderWindow().draw(m_carAngleText);
			CoreWindow::getRenderWindow().draw(m_carAngleActiveText);
			CoreWindow::getRenderWindow().draw(m_carAngleHelpText);
			break;
		}
		case ActiveMode::FINISH_LINE:
		{
			if (m_insertFinishLine)
			{
				m_drawableFinishLine.setStartPoint(m_finishLineBeggining);
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				sf::Vector2f correctPosition;
				correctPosition.x = static_cast<float>(mousePosition.x);
				correctPosition.y = static_cast<float>(mousePosition.y);
				correctPosition += CoreWindow::getViewOffset();
				m_drawableFinishLine.setEndPoint(correctPosition);
				m_drawableFinishLine.draw();
			}
			
			CoreWindow::getRenderWindow().draw(m_finishLineSubmodeText);
			CoreWindow::getRenderWindow().draw(m_finishLineSubmodeActiveText);
			CoreWindow::getRenderWindow().draw(m_finishLineSubmodeHelpText);
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
	CoreWindow::getRenderWindow().draw(m_saveStatusText);
}