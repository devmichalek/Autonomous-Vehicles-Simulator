#include "StateEditor.hpp"
#include "CoreWindow.hpp"
#include "DrawableBuilder.hpp"
#include "TypeObserver.hpp"
#include "FunctionObserver.hpp"

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
				m_edgeSubmodeText.setVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
				break;
			case ActiveMode::CAR:
				m_carSubmode = CarSubmode::INSERT;
				m_carSubmodeText.setVariableText(m_carSubmodeMap[m_carSubmode]);
				break;
			case ActiveMode::FINISH_LINE:
				m_finishLineSubmode = FinishLineSubmode::INSERT;
				m_finishLineSubmodeText.setVariableText(m_finishLineSubmodeMap[m_finishLineSubmode]);
				break;
		}

		m_activeMode = activeMode;
		m_activeModeText.setVariableText(m_activeModeMap[m_activeMode]);
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
							m_numberOfEdges = m_edges.size();
							m_saveStatus = SaveStatus::OUT_OF_DATE;
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
									m_saveStatus = SaveStatus::OUT_OF_DATE;
									m_edges.erase(m_edges.begin() + i);
									--size;
									--i;
								}
							}
							m_numberOfEdges = m_edges.size();
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
						m_saveStatus = SaveStatus::OUT_OF_DATE;
						break;
					}
					case CarSubmode::REMOVE:
					{
						if (m_drawableCar.inside(correctPosition))
						{
							m_drawCar = false;
							m_saveStatus = SaveStatus::OUT_OF_DATE;
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
							m_saveStatus = SaveStatus::OUT_OF_DATE;
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
					m_edgeSubmodeText.setVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
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
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				{
					m_drawableCar.rotate(1.0);
					m_drawableCar.update();
				}

				if (m_carSubmode != mode)
				{
					m_carSubmode = mode;
					m_carSubmodeText.setVariableText(m_carSubmodeMap[m_carSubmode]);
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
					m_finishLineSubmodeText.setVariableText(m_finishLineSubmodeMap[m_finishLineSubmode]);
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
		m_movementTimer.increment();
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
	{
		m_movementTimer.decrement();
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(static_cast<float>(-m_movementTimer.value() * elapsedTime), 0));
		CoreWindow::getRenderWindow().setView(view);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(static_cast<float>(m_movementTimer.value() * elapsedTime), 0));
		CoreWindow::getRenderWindow().setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, static_cast<float>(-m_movementTimer.value() * elapsedTime)));
		CoreWindow::getRenderWindow().setView(view);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		auto& view = CoreWindow::getView();
		view.move(sf::Vector2f(0, static_cast<float>(m_movementTimer.value() * elapsedTime)));
		CoreWindow::getRenderWindow().setView(view);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			if (!m_saveKeysPressed)
			{
				m_saveKeysPressed = true;
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
				m_saveText.showStatusText();
				m_saveText.setVariableText(std::get<1>(m_saveStatusMap[m_saveStatus]));
				m_saveText.setStatusText(std::get<0>(m_saveStatusMap[m_saveStatus]));
				m_saveText.setStatusTextColor(std::get<2>(m_saveStatusMap[m_saveStatus]));
			}
		}
		else
			m_saveKeysPressed = false;
	}
	else
		m_saveKeysPressed = false;

	m_activeModeText.update();
	m_viewOffsetXText.update();
	m_viewOffsetYText.update();
	m_movementText.update();
	m_saveText.update();
	m_edgeSubmodeText.update();
	m_edgeCountText.update();
	m_carSubmodeText.update();
	m_carAngleText.update();
	m_finishLineSubmodeText.update();
}

void StateEditor::load()
{
	m_activeModeMap[ActiveMode::EDGE] = "Edge mode";
	m_activeModeMap[ActiveMode::CAR] = "Car mode";
	m_activeModeMap[ActiveMode::FINISH_LINE] = "Finish line mode";

	m_edgeSubmodeMap[EdgeSubmode::GLUED_INSERT] = "Glued insert mode";
	m_edgeSubmodeMap[EdgeSubmode::REMOVE] = "Remove mode";

	m_carSubmodeMap[CarSubmode::INSERT] = "Insert mode";
	m_carSubmodeMap[CarSubmode::REMOVE] = "Remove mode";

	m_finishLineSubmodeMap[FinishLineSubmode::INSERT] = "Insert mode";

	m_saveStatusMap[SaveStatus::UP_TO_DATE] = std::tuple("Success: Changes were saved", "Up to date", sf::Color::Green);
	m_saveStatusMap[SaveStatus::OUT_OF_DATE] = std::tuple("Warning: Changes not saved", "Out of date", sf::Color(0, 0, 0, 0));
	m_saveStatusMap[SaveStatus::ERROR_NO_EDGES_POSITIONED] = std::tuple("Error: No edges positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_CAR_POSITIONED] = std::tuple("Error: Car is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_NO_FINISH_LINE_POSITIONED] = std::tuple("Error: Finish line is not positioned!", "Out of date", sf::Color::Red);
	m_saveStatusMap[SaveStatus::ERROR_CANNOT_OPEN_FILE] = std::tuple("Error: Cannot open file!", "Out of date", sf::Color::Red);

	// Set consistent texts
	m_activeModeText.setConsistentText("Active mode:");
	m_movementText.setConsistentText("Movement:");
	m_viewOffsetXText.setConsistentText("View offset x:");
	m_viewOffsetYText.setConsistentText("View offset y:");
	m_saveText.setConsistentText("Save status:");
	m_edgeSubmodeText.setConsistentText("Current mode:");
	m_edgeCountText.setConsistentText("Edge count:");
	m_carSubmodeText.setConsistentText("Current mode:");
	m_carAngleText.setConsistentText("Car angle:");
	m_finishLineSubmodeText.setConsistentText("Current mode:");

	// Set variable texts
	m_textFunctions.reserve(16U);
	m_activeModeText.setVariableText(m_activeModeMap[m_activeMode]);
	m_movementText.setObserver(new TypeObserver<double, int>(m_movementTimer.value(), 0.05));
	m_textFunctions.push_back([&] { return std::to_string(int(CoreWindow::getViewOffset().x)); });
	m_viewOffsetXText.setObserver(new FunctionObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return std::to_string(int(CoreWindow::getViewOffset().y)); });
	m_viewOffsetYText.setObserver(new FunctionObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return std::get<1>(m_saveStatusMap[m_saveStatus]); });
	m_saveText.setObserver(new FunctionObserver<std::string>(m_textFunctions.back(), 0.5));
	m_edgeSubmodeText.setVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
	m_edgeCountText.setObserver(new TypeObserver(m_numberOfEdges));
	m_carSubmodeText.setVariableText(m_carSubmodeMap[m_carSubmode]);
	m_textFunctions.push_back([&] { long long angle = static_cast<long long>(m_drawableCar.getAngle()) % 360;
									return std::to_string(angle < 0 ? (angle + 360) : angle); } );
	m_carAngleText.setObserver(new FunctionObserver<std::string>(m_textFunctions.back(), 0.2));
	m_finishLineSubmodeText.setVariableText(m_finishLineSubmodeMap[m_finishLineSubmode]);

	// Set information texts
	m_activeModeText.setInformationText("| Keys: [F1] [F2] [F3]");
	m_movementText.setInformationText("| Keys: [+] [-]");
	m_saveText.setInformationText("| Keys: [Ctrl] + [S]");
	m_edgeSubmodeText.setInformationText("| Keys: [1] [2]");
	m_carSubmodeText.setInformationText("| Keys: [1] [2]");
	m_carAngleText.setInformationText("| Keys: [Z] [X]");
	m_finishLineSubmodeText.setInformationText("| Keys: [1]");
	
	// Set positions
	m_activeModeText.setPosition(0.0039, 0.07, 0.15, 0.022 * 0);
	m_movementText.setPosition(0.0039, 0.07, 0.15, 0.022 * 1);
	m_viewOffsetXText.setPosition(0.0039, 0.07, 0.022 * 2);
	m_viewOffsetYText.setPosition(0.0039, 0.07, 0.022 * 3);
	m_saveText.setPosition(0.0039, 0.07, 0.15, 0.25, 0.022 * 4);
	m_edgeSubmodeText.setPosition(0.0039, 0.07, 0.15, 1 - (0.022 * 1));
	m_edgeCountText.setPosition(0.0039, 0.07, 1 - (0.022 * 2));
	m_carSubmodeText.setPosition(0.0039, 0.07, 0.15, 1 - (0.022 * 1));
	m_carAngleText.setPosition(0.0039, 0.07, 0.15, 1 - (0.022 * 2));
	m_finishLineSubmodeText.setPosition(0.0039, 0.07, 0.15, 1 - (0.022 * 1));
}

void StateEditor::draw()
{
	if (m_drawCar)
	{
		m_drawableCar.drawBody();
		m_drawableCar.drawBeams();
	}

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

			m_edgeSubmodeText.draw();
			m_edgeCountText.draw();
			break;
		}
		case ActiveMode::CAR:
		{
			m_carSubmodeText.draw();
			m_carAngleText.draw();
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
			
			m_finishLineSubmodeText.draw();
			break;
		}
		default:
			break;
	}

	m_activeModeText.draw();
	m_movementText.draw();
	m_viewOffsetXText.draw();
	m_viewOffsetYText.draw();
	m_saveText.draw();
}