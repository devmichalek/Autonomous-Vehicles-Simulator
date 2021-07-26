#include "StateEditor.hpp"
#include "CoreWindow.hpp"
#include "DrawableBuilder.hpp"

void StateEditor::setEdgeCountActiveText()
{
	m_edgeCountActiveText.setString(std::to_string(m_edges.size()));
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
	m_edgeSubmodeText.setPosition(sf::Vector2f(textX, textY));
	m_edgeSubmodeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_edgeSubmodeHelpText.setPosition(sf::Vector2f(helpTextX, textY));
	m_checkpointSubmodeText.setPosition(sf::Vector2f(textX, textY));
	m_checkpointSubmodeActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	m_checkpointSubmodeHelpText.setPosition(sf::Vector2f(helpTextX, textY));

	textY -= h;
	m_edgeCountText.setPosition(sf::Vector2f(textX, textY));
	m_edgeCountActiveText.setPosition(sf::Vector2f(activeTextX, textY));
	
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
	helpTextX = viewOffset.x + float(CoreWindow::getSize().x) / 6.8f;
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
			if (m_edges.empty())
			{
				result = SaveStatus::ERROR_NO_EDGES_POSITIONED;
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
			for (auto& i : m_edges)
				builder.addEdge(i);
			Edge edge = { m_drawableFinishLine.getStartPoint(), m_drawableFinishLine.getEndPoint() };
			builder.addFinishLine(edge);

			for (auto& i : m_checkpoints)
				builder.addCheckpoint(i);

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

void StateEditor::setActiveMode(ActiveMode activeMode)
{
	if (m_activeMode != activeMode)
	{
		switch (m_activeMode)
		{
			case ActiveMode::EDGE:
				m_edgeSubmode = EdgeSubmode::GLUED_INSERT;
				m_insertEdge = false;
				m_removeEdge = false;
				m_edgeSubmodeActiveText.setString(m_edgeSubmodeMap[m_edgeSubmode]);
				break;
			case ActiveMode::CAR:
				m_carSubmode = CarSubmode::INSERT;
				break;
			case ActiveMode::FINISH_LINE:
				m_finishLineSubmode = FinishLineSubmode::INSERT;
				break;
			case ActiveMode::CHECKPOINT:
				m_checkpointSubmode = CheckpointSubmode::GLUED_INSERT;
				m_insertCheckpoint = false;
				m_removeCheckpoint = false;
				m_checkpointSubmodeActiveText.setString(m_checkpointSubmodeMap[m_checkpointSubmode]);
				break;
		}

		m_activeMode = activeMode;
		m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
	}
}

void StateEditor::capture()
{
	if ((CoreWindow::getEvent().type == sf::Event::MouseButtonPressed ||
		(CoreWindow::getEvent().type == sf::Event::KeyPressed && CoreWindow::getEvent().key.code == sf::Keyboard::Space)) && !m_spaceKeyPressed)
	{
		m_spaceKeyPressed = true;
		sf::Vector2i mousePosition = CoreWindow::getMousePosition();
		sf::Vector2f viewOffset = CoreWindow::getViewOffset();
		sf::Vector2f correctPosition;
		correctPosition.x = static_cast<float>(mousePosition.x + viewOffset.x);
		correctPosition.y = static_cast<float>(mousePosition.y + viewOffset.y);

		switch (m_activeMode)
		{
			case ActiveMode::EDGE:
			{
				switch (m_edgeSubmode)
				{
					case EdgeSubmode::GLUED_INSERT:
					{
						if (m_insertEdge)
						{
							if (!m_edges.empty())
							{
								size_t size = m_edges.size() - 1;
								for (size_t i = 0; i < size; ++i)
								{
									if (Intersect(m_edges[i], m_edgeBeggining, correctPosition))
									{
										correctPosition = m_edges[i][0];
										m_insertEdge = false;
										break;
									}
								}
							}

							Edge newEdge;
							newEdge[0] = m_edgeBeggining;
							newEdge[1] = correctPosition;
							m_edges.push_back(newEdge);
							setEdgeCountActiveText();
							setOutOfDate();
						}
						else
							m_insertEdge = true;

						m_edgeBeggining = correctPosition;
						break;
					}
					case EdgeSubmode::REMOVE:
					{
						if (m_removeEdge)
						{
							size_t size = m_edges.size();
							for (size_t i = 0; i < size; ++i)
							{
								if (Intersect(m_edges[i], m_edgeBeggining, correctPosition))
								{
									setOutOfDate();
									m_edges.erase(m_edges.begin() + i);
									--size;
									--i;
								}
							}
							setEdgeCountActiveText();
						}
						else
							m_edgeBeggining = correctPosition;

						m_removeEdge = !m_removeEdge;
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
						if (m_drawableCar.inside(correctPosition))
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
			case ActiveMode::CHECKPOINT:
			{
				switch (m_checkpointSubmode)
				{
					case CheckpointSubmode::GLUED_INSERT:
					{
						if (m_insertCheckpoint)
						{
							Edge newEdge;
							newEdge[0] = m_checkpoints.empty() ? m_checkpointBeggining : m_checkpoints.back()[1];
							newEdge[1] = correctPosition;
							m_checkpoints.push_back(newEdge);
							setOutOfDate();
						}
						else
							m_insertCheckpoint = true;

						m_checkpointBeggining = correctPosition;
						break;
					}
					case CheckpointSubmode::REMOVE:
					{
						if (m_removeCheckpoint)
						{
							size_t size = m_checkpoints.size();
							size_t lastIndex = size;
							for (size_t i = size - 1; i < size; --i)
							{
								if (Intersect(m_checkpoints[i], m_checkpointBeggining, correctPosition))
									lastIndex = i;
							}

							setOutOfDate();
							m_checkpoints.erase(m_checkpoints.begin() + lastIndex, m_checkpoints.begin() + size);
						}
						else
							m_checkpointBeggining = correctPosition;

						m_removeCheckpoint = !m_removeCheckpoint;
						break;
					}
				}
				break;
			}
			default:
				break;
		}
	}
	else if (CoreWindow::getEvent().type == sf::Event::MouseButtonReleased ||
			 (CoreWindow::getEvent().type == sf::Event::KeyReleased && CoreWindow::getEvent().key.code == sf::Keyboard::Space))
		m_spaceKeyPressed = false;
}

void StateEditor::update()
{
	switch (m_activeMode)
	{
		case ActiveMode::EDGE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
				setActiveMode(ActiveMode::CAR);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				setActiveMode(ActiveMode::FINISH_LINE);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				setActiveMode(ActiveMode::CHECKPOINT);
			else
			{
				EdgeSubmode mode = m_edgeSubmode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = EdgeSubmode::GLUED_INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
						 sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
				{
					mode = EdgeSubmode::REMOVE;
				}

				if (m_edgeSubmode != mode)
				{
					m_edgeSubmode = mode;
					m_insertEdge = false;
					m_removeEdge = false;
					m_edgeSubmodeActiveText.setString(m_edgeSubmodeMap[m_edgeSubmode]);
				}
			}

			break;
		}
		case ActiveMode::CAR:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
				setActiveMode(ActiveMode::EDGE);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				setActiveMode(ActiveMode::FINISH_LINE);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				setActiveMode(ActiveMode::CHECKPOINT);
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
					m_drawableCar.rotate(0.0);
					m_drawableCar.update();
					setCarAngleActiveText();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				{
					m_drawableCar.rotate(1.0);
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
				setActiveMode(ActiveMode::EDGE);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
				setActiveMode(ActiveMode::CAR);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				setActiveMode(ActiveMode::CHECKPOINT);
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
		case ActiveMode::CHECKPOINT:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
				setActiveMode(ActiveMode::EDGE);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
				setActiveMode(ActiveMode::CAR);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				setActiveMode(ActiveMode::FINISH_LINE);
			else
			{
				CheckpointSubmode mode = m_checkpointSubmode;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
				{
					mode = CheckpointSubmode::GLUED_INSERT;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
					sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
				{
					mode = CheckpointSubmode::REMOVE;
				}

				if (m_checkpointSubmode != mode)
				{
					m_checkpointSubmode = mode;
					m_insertCheckpoint = false;
					m_removeCheckpoint = false;
					m_checkpointSubmodeActiveText.setString(m_checkpointSubmodeMap[m_checkpointSubmode]);
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
	m_activeModeMap[ActiveMode::EDGE] = "Edge mode";
	m_activeModeMap[ActiveMode::CAR] = "Car mode";
	m_activeModeMap[ActiveMode::FINISH_LINE] = "Finish line mode";
	m_activeModeMap[ActiveMode::CHECKPOINT] = "Checkpoint mode";

	m_edgeSubmodeMap[EdgeSubmode::GLUED_INSERT] = "Glued insert mode";
	m_edgeSubmodeMap[EdgeSubmode::REMOVE] = "Remove mode";

	m_carSubmodeMap[CarSubmode::INSERT] = "Insert mode";
	m_carSubmodeMap[CarSubmode::REMOVE] = "Remove mode";

	m_finishLineSubmodeMap[FinishLineSubmode::INSERT] = "Insert mode";

	m_checkpointSubmodeMap[CheckpointSubmode::GLUED_INSERT] = "Glued insert mode";
	m_checkpointSubmodeMap[CheckpointSubmode::REMOVE] = "Remove mode";

	m_saveStatusMap[SaveStatus::UP_TO_DATE] = std::tuple("Success: Changes were saved", "Up to date", sf::Color::Green);
	m_saveStatusMap[SaveStatus::OUT_OF_DATE] = std::tuple("Warning: Changes not saved", "Out of date", sf::Color(0, 0, 0, 0));
	m_saveStatusMap[SaveStatus::ERROR_NO_EDGES_POSITIONED] = std::tuple("Error: No edges positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_CAR_POSITIONED] = std::tuple("Error: Car is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_FINISH_LINE_POSITIONED] = std::tuple("Error: Finish line is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_CANNOT_OPEN_FILE] = std::tuple("Error: Cannot open file!", "Out of date", sf::Color::Red);

	if (m_font.loadFromFile("consola.ttf"))
	{
		m_edgeSubmodeText.setFont(m_font);
		m_edgeSubmodeActiveText.setFont(m_font);
		m_edgeSubmodeHelpText.setFont(m_font);
		m_carSubmodeText.setFont(m_font);
		m_carSubmodeActiveText.setFont(m_font);
		m_carSubmodeHelpText.setFont(m_font);
		m_finishLineSubmodeText.setFont(m_font);
		m_finishLineSubmodeActiveText.setFont(m_font);
		m_finishLineSubmodeHelpText.setFont(m_font);
		m_checkpointSubmodeText.setFont(m_font);
		m_checkpointSubmodeActiveText.setFont(m_font);
		m_checkpointSubmodeHelpText.setFont(m_font);
		m_edgeCountText.setFont(m_font);
		m_edgeCountActiveText.setFont(m_font);
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
		m_edgeSubmodeActiveText.setFillColor(activeColor);
		m_carSubmodeActiveText.setFillColor(activeColor);
		m_finishLineSubmodeActiveText.setFillColor(activeColor);
		m_checkpointSubmodeActiveText.setFillColor(activeColor);
		m_edgeCountActiveText.setFillColor(activeColor);
		m_carAngleActiveText.setFillColor(activeColor);
		m_activeModeActiveText.setFillColor(activeColor);
		m_movementActiveText.setFillColor(activeColor);
		m_viewOffsetXActiveText.setFillColor(activeColor);
		m_viewOffsetYActiveText.setFillColor(activeColor);
		m_saveActiveText.setFillColor(activeColor);

		unsigned int characterSize = CoreWindow::getSize().x / 116;
		m_edgeSubmodeText.setCharacterSize(characterSize);
		m_edgeSubmodeActiveText.setCharacterSize(characterSize);
		m_edgeSubmodeHelpText.setCharacterSize(characterSize);
		m_carSubmodeText.setCharacterSize(characterSize);
		m_carSubmodeActiveText.setCharacterSize(characterSize);
		m_carSubmodeHelpText.setCharacterSize(characterSize);
		m_finishLineSubmodeText.setCharacterSize(characterSize);
		m_finishLineSubmodeActiveText.setCharacterSize(characterSize);
		m_finishLineSubmodeHelpText.setCharacterSize(characterSize);
		m_checkpointSubmodeText.setCharacterSize(characterSize);
		m_checkpointSubmodeActiveText.setCharacterSize(characterSize);
		m_checkpointSubmodeHelpText.setCharacterSize(characterSize);
		m_edgeCountText.setCharacterSize(characterSize);
		m_edgeCountActiveText.setCharacterSize(characterSize);
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

		m_edgeSubmodeText.setString("Current mode:");
		m_edgeSubmodeActiveText.setString(m_edgeSubmodeMap[m_edgeSubmode]);
		m_edgeSubmodeHelpText.setString("| Keys: [1] [2]");
		m_carSubmodeText.setString("Current mode:");
		m_carSubmodeActiveText.setString(m_carSubmodeMap[m_carSubmode]);
		m_carSubmodeHelpText.setString("| Keys: [1] [2]");
		m_finishLineSubmodeText.setString("Current mode:");
		m_finishLineSubmodeActiveText.setString(m_finishLineSubmodeMap[m_finishLineSubmode]);
		m_finishLineSubmodeHelpText.setString("| Keys: [1]");
		m_checkpointSubmodeText.setString("Current mode:");
		m_checkpointSubmodeActiveText.setString(m_checkpointSubmodeMap[m_checkpointSubmode]);
		m_checkpointSubmodeHelpText.setString("| Keys: [1] [2]");
		m_edgeCountText.setString("Edge count:");
		setEdgeCountActiveText();
		m_carAngleText.setString("Car angle:");
		setCarAngleActiveText();
		m_carAngleHelpText.setString("| Keys: [Z] [X]");
		m_activeModeText.setString("Active mode:");
		m_activeModeActiveText.setString(m_activeModeMap[m_activeMode]);
		m_activeModeHelpText.setString("| Keys: [F1] [F2] [F3] [F4]");
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
	m_line[1].color = m_line[0].color;
	for (const auto& i : m_edges)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	m_line[0].color = sf::Color(0, 255, 0, 128);
	m_line[1].color = m_line[0].color;
	for (const auto& i : m_checkpoints)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	switch (m_activeMode)
	{
		case ActiveMode::EDGE:
		{
			if (m_insertEdge)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_edgeBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				m_line[0].color = sf::Color::White;
				m_line[1].color = m_line[0].color;
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}
			else if (m_removeEdge)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_edgeBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				m_line[0].color = sf::Color::Red;
				m_line[1].color = m_line[0].color;
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}

			CoreWindow::getRenderWindow().draw(m_edgeSubmodeText);
			CoreWindow::getRenderWindow().draw(m_edgeSubmodeActiveText);
			CoreWindow::getRenderWindow().draw(m_edgeSubmodeHelpText);
			CoreWindow::getRenderWindow().draw(m_edgeCountText);
			CoreWindow::getRenderWindow().draw(m_edgeCountActiveText);
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
		case ActiveMode::CHECKPOINT:
		{
			if (m_insertCheckpoint)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_checkpoints.empty() ? m_checkpointBeggining : m_checkpoints.back()[1];
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				m_line[0].color = sf::Color(0, 255, 0, 128);
				m_line[1].color = m_line[0].color;
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}
			else if (m_removeCheckpoint)
			{
				sf::Vector2i mousePosition = CoreWindow::getMousePosition();
				m_line[0].position = m_checkpointBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::getViewOffset();
				m_line[0].color = sf::Color::Red;
				m_line[1].color = sf::Color::Red;
				CoreWindow::getRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}

			CoreWindow::getRenderWindow().draw(m_checkpointSubmodeText);
			CoreWindow::getRenderWindow().draw(m_checkpointSubmodeActiveText);
			CoreWindow::getRenderWindow().draw(m_checkpointSubmodeHelpText);
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