#include "StateMapEditor.hpp"
#include "CoreWindow.hpp"
#include "TypeTimerObserver.hpp"
#include "FunctionTimerObserver.hpp"
#include "CoreLogger.hpp"

void StateMapEditor::SetActiveMode(ActiveMode activeMode)
{
	if (m_activeMode != activeMode)
	{
		switch (m_activeMode)
		{
			case ActiveMode::EDGE:
				m_edgeSubmode = EdgeSubmode::GLUED_INSERT;
				m_insertEdge = false;
				m_removeEdge = false;
				m_edgeSubmodeText.SetVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
				break;
			case ActiveMode::CAR:
				m_carSubmode = CarSubmode::INSERT;
				m_carSubmodeText.SetVariableText(m_carSubmodeMap[m_carSubmode]);
				break;
		}

		m_activeMode = activeMode;
		m_activeModeText.SetVariableText(m_activeModeMap[m_activeMode]);
	}
}

StateMapEditor::StateMapEditor() :
	m_movementTimer(600.0, 1800.0, 1.0, 200.0)
{
	m_activeMode = ActiveMode::EDGE;
	m_edgeSubmode = EdgeSubmode::GLUED_INSERT;
	m_carSubmode = CarSubmode::INSERT;
	m_line[0].color = sf::Color::White;
	m_line[1].color = sf::Color::White;
	m_edges.reserve(1024);
	m_insertEdge = false;
	m_removeEdge = false;
	m_upToDate = false;
	m_carPositioned = false;
	m_drawableCar.setAngle(0.0);
	m_drawableCar.setCenter(sf::Vector2f(0.0f, 0.0f));
	m_spaceKeyPressed = false;
	m_textFunctions.reserve(16U);
}

StateMapEditor::~StateMapEditor()
{
}

void StateMapEditor::Reload()
{
	// Reset internal states
	m_activeMode = ActiveMode::EDGE;
	m_edgeSubmode = EdgeSubmode::GLUED_INSERT;
	m_carSubmode = CarSubmode::INSERT;
	m_edges.clear();
	m_insertEdge = false;
	m_removeEdge = false;
	m_edgeBeggining = sf::Vector2f(0.0, 0.0);
	m_movementTimer.Reset();
	m_upToDate = false;
	m_carPositioned = false;
	m_drawableCar.setAngle(0.0);
	m_drawableCar.setCenter(sf::Vector2f(0.0f, 0.0f));
	m_builder.Clear();
	m_spaceKeyPressed = false;

	// Reset texts
	m_activeModeText.SetVariableText(m_activeModeMap[m_activeMode]);
	m_movementText.ResetObserverTimer();
	m_viewOffsetXText.ResetObserverTimer();
	m_viewOffsetYText.ResetObserverTimer();
	m_filenameText.Reset();
	m_filenameText.ResetObserverTimer();
	m_edgeSubmodeText.SetVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
	m_edgeCountText.ResetObserverTimer();
	m_carSubmodeText.SetVariableText(m_carSubmodeMap[m_carSubmode]);
	m_carAngleText.ResetObserverTimer();

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateMapEditor::Capture()
{
	if ((CoreWindow::GetEvent().type == sf::Event::MouseButtonPressed ||
		(CoreWindow::GetEvent().type == sf::Event::KeyPressed && CoreWindow::GetEvent().key.code == sf::Keyboard::Space)) && !m_spaceKeyPressed)
	{
		m_spaceKeyPressed = true;
		sf::Vector2i mousePosition = CoreWindow::GetMousePosition();
		sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
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
								Edge temporaryEdge = { m_edgeBeggining, correctPosition };
								sf::Vector2f intersectionPoint;
								for (size_t i = 0; i < size; ++i)
								{
									if (GetIntersectionPoint(m_edges[i], temporaryEdge, intersectionPoint))
									{
										auto segment = Distance(m_edges[i][0], intersectionPoint);
										auto length = Distance(m_edges[i]);
										double percentage = segment / length;
										if (percentage > 0.01 && percentage < 0.5)
											correctPosition = m_edges[i][0];
										else if (percentage > 0.5 && percentage < 0.99)
											correctPosition = m_edges[i][1];
										else
											break;
										m_insertEdge = false;
										break;
									}
								}
							}

							Edge newEdge;
							newEdge[0] = m_edgeBeggining;
							newEdge[1] = correctPosition;
							m_edges.push_back(newEdge);
							m_upToDate = false;
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
									m_upToDate = false;
									m_edges.erase(m_edges.begin() + i);
									--size;
									--i;
								}
							}
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
						m_carPositioned = true;
						m_drawableCar.setCenter(correctPosition);
						m_drawableCar.update();
						m_upToDate = false;
						break;
					}
					case CarSubmode::REMOVE:
					{
						if (m_drawableCar.inside(correctPosition))
						{
							m_carPositioned = false;
							m_upToDate = false;
						}
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
	else if (CoreWindow::GetEvent().type == sf::Event::MouseButtonReleased ||
			 (CoreWindow::GetEvent().type == sf::Event::KeyReleased && CoreWindow::GetEvent().key.code == sf::Keyboard::Space))
		m_spaceKeyPressed = false;

	m_filenameText.Capture();
}

void StateMapEditor::Update()
{
	if (m_filenameText.IsReading())
	{
		bool success = m_builder.Load(m_filenameText.GetFilename());
		auto status = m_builder.GetLastOperationStatus();
		m_filenameText.ShowStatusText();
		if (success)
		{
			m_filenameText.SetSuccessStatusText(status.second);
			m_edges = m_builder.GetEdges();
			m_carPositioned = true;
			auto data = m_builder.GetCar();
			m_drawableCar.setCenter(data.first);
			m_drawableCar.setAngle(data.second);
			m_drawableCar.update();
			m_upToDate = true;
		}
		else
			m_filenameText.SetErrorStatusText(status.second);
	}
	else if (m_filenameText.IsWriting())
	{
		if (!m_upToDate)
		{
			m_builder.Clear();
			if (m_carPositioned)
				m_builder.AddCar(m_drawableCar.getAngle(), m_drawableCar.getCenter());
			for (auto& i : m_edges)
				m_builder.AddEdge(i);
			bool success = m_builder.Save(m_filenameText.GetFilename());
			std::string message;
			auto status = m_builder.GetLastOperationStatus();
			m_filenameText.ShowStatusText();
			if (success)
				m_filenameText.SetSuccessStatusText(status.second);
			else
				m_filenameText.SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (!m_filenameText.IsRenaming())
	{
		switch (m_activeMode)
		{
		case ActiveMode::EDGE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
				SetActiveMode(ActiveMode::CAR);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) ||
				sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
			{
				if (!m_edges.empty() && m_edgeSubmode == EdgeSubmode::GLUED_INSERT)
				{
					sf::Vector2i mousePosition = CoreWindow::GetMousePosition();
					sf::Vector2f viewOffset = CoreWindow::GetViewOffset();
					sf::Vector2f correctPosition;
					correctPosition.x = static_cast<float>(mousePosition.x + viewOffset.x);
					correctPosition.y = static_cast<float>(mousePosition.y + viewOffset.y);
					m_edgeBeggining = correctPosition;
					m_insertEdge = true;

					// Find closest point to mouse position
					size_t index = 0;
					double distance = std::numeric_limits<double>::max();
					for (auto& edge : m_edges)
					{
						double beginningDistance = Distance(correctPosition, edge[0]);
						if (beginningDistance < distance)
						{
							distance = beginningDistance;
							m_edgeBeggining = edge[0];
						}

						double endDistance = Distance(correctPosition, edge[1]);
						if (endDistance < distance)
						{
							distance = endDistance;
							m_edgeBeggining = edge[1];
						}
					}
				}
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				m_insertEdge = false;
				m_removeEdge = false;
			}
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
					m_edgeSubmodeText.SetVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
				}
			}

			break;
		}
		case ActiveMode::CAR:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
				SetActiveMode(ActiveMode::EDGE);
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
				else if (m_carPositioned)
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
					{
						m_drawableCar.rotate(0.0);
						m_drawableCar.update();
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
					{
						m_drawableCar.rotate(1.0);
						m_drawableCar.update();
					}
				}

				if (m_carSubmode != mode)
				{
					m_carSubmode = mode;
					m_carSubmodeText.SetVariableText(m_carSubmodeMap[m_carSubmode]);
				}
			}

			break;
		}
		default:
			break;
		}

		float elapsedTime = float(CoreWindow::GetElapsedTime());
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
		{
			m_movementTimer.Increment();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
		{
			m_movementTimer.Decrement();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			auto& view = CoreWindow::GetView();
			view.move(sf::Vector2f(static_cast<float>(-m_movementTimer.Value() * elapsedTime), 0));
			CoreWindow::GetRenderWindow().setView(view);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			auto& view = CoreWindow::GetView();
			view.move(sf::Vector2f(static_cast<float>(m_movementTimer.Value() * elapsedTime), 0));
			CoreWindow::GetRenderWindow().setView(view);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			auto& view = CoreWindow::GetView();
			view.move(sf::Vector2f(0, static_cast<float>(-m_movementTimer.Value() * elapsedTime)));
			CoreWindow::GetRenderWindow().setView(view);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			auto& view = CoreWindow::GetView();
			view.move(sf::Vector2f(0, static_cast<float>(m_movementTimer.Value() * elapsedTime)));
			CoreWindow::GetRenderWindow().setView(view);
		}
	}

	m_activeModeText.Update();
	m_viewOffsetXText.Update();
	m_viewOffsetYText.Update();
	m_movementText.Update();
	m_filenameText.Update();
	m_edgeSubmodeText.Update();
	m_edgeCountText.Update();
	m_carSubmodeText.Update();
	m_carAngleText.Update();
}

bool StateMapEditor::Load()
{
	m_activeModeMap[ActiveMode::EDGE] = "Edge mode";
	m_activeModeMap[ActiveMode::CAR] = "Car mode";

	m_edgeSubmodeMap[EdgeSubmode::GLUED_INSERT] = "Glued insert mode";
	m_edgeSubmodeMap[EdgeSubmode::REMOVE] = "Remove mode";

	m_carSubmodeMap[CarSubmode::INSERT] = "Insert mode";
	m_carSubmodeMap[CarSubmode::REMOVE] = "Remove mode";

	// Set consistent texts
	m_activeModeText.SetConsistentText("Active mode:");
	m_movementText.SetConsistentText("Movement:");
	m_viewOffsetXText.SetConsistentText("View offset x:");
	m_viewOffsetYText.SetConsistentText("View offset y:");
	m_edgeSubmodeText.SetConsistentText("Current mode:");
	m_edgeCountText.SetConsistentText("Edge count:");
	m_carSubmodeText.SetConsistentText("Current mode:");
	m_carAngleText.SetConsistentText("Car angle:");

	// Set variable texts
	m_activeModeText.SetVariableText(m_activeModeMap[m_activeMode]);
	m_movementText.SetObserver(new TypeTimerObserver<double, int>(m_movementTimer.Value(), 0.05));
	m_textFunctions.push_back([&] { return std::to_string(int(CoreWindow::GetViewOffset().x)); });
	m_viewOffsetXText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return std::to_string(int(CoreWindow::GetViewOffset().y)); });
	m_viewOffsetYText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_edgeSubmodeText.SetVariableText(m_edgeSubmodeMap[m_edgeSubmode]);
	m_textFunctions.push_back([&] { return std::to_string(m_edges.size()); });
	m_edgeCountText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.5));
	m_carSubmodeText.SetVariableText(m_carSubmodeMap[m_carSubmode]);
	m_textFunctions.push_back([&] { double angle = double((long long)(m_drawableCar.getAngle()) % 360);
									return std::to_string(CastAtan2ToFullAngle(angle)); } );
	m_carAngleText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.2));

	// Set information texts
	m_activeModeText.SetInformationText("| [F1] [F2]");
	m_movementText.SetInformationText("| [+] [-]");
	m_viewOffsetXText.SetInformationText("| [Left] [Right]");
	m_viewOffsetYText.SetInformationText("| [Up] [Down]");
	m_edgeSubmodeText.SetInformationText("| [1] [2] [Alt] [Esc]");
	m_carSubmodeText.SetInformationText("| [1] [2]");
	m_carAngleText.SetInformationText("| [Z] [X]");
	
	// Set text positions
	m_activeModeText.SetPosition({ FontContext::Component(0), {3}, {7}, {0} });
	m_movementText.SetPosition({ FontContext::Component(0), {3}, {7}, {1} });
	m_viewOffsetXText.SetPosition({ FontContext::Component(0), {3}, {7}, {2} });
	m_viewOffsetYText.SetPosition({ FontContext::Component(0), {3}, {7}, {3} });
	m_filenameText.SetPosition({ FontContext::Component(0), {3}, {7}, {16}, {4} });
	m_edgeSubmodeText.SetPosition({ FontContext::Component(0), {3}, {7}, {1, true} });
	m_edgeCountText.SetPosition({ FontContext::Component(0), {3}, {2, true} });
	m_carSubmodeText.SetPosition({ FontContext::Component(0), {3}, {7}, {1, true} });
	m_carAngleText.SetPosition({ FontContext::Component(0), {3}, {7}, {2, true} });

	CoreLogger::PrintSuccess("State \"Map Editor\" dependencies loaded correctly");
	return true;
}

void StateMapEditor::Draw()
{
	if (m_carPositioned)
	{
		m_drawableCar.drawBody();
		m_drawableCar.drawBeams();
	}

	m_line[0].color = sf::Color::White;
	m_line[1].color = m_line[0].color;
	for (const auto& i : m_edges)
	{
		m_line[0].position = i[0];
		m_line[1].position = i[1];
		CoreWindow::GetRenderWindow().draw(m_line.data(), 2, sf::Lines);
	}

	switch (m_activeMode)
	{
		case ActiveMode::EDGE:
		{
			if (m_insertEdge)
			{
				sf::Vector2i mousePosition = CoreWindow::GetMousePosition();
				m_line[0].position = m_edgeBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::GetViewOffset();
				m_line[0].color = sf::Color::White;
				m_line[1].color = m_line[0].color;
				CoreWindow::GetRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}
			else if (m_removeEdge)
			{
				sf::Vector2i mousePosition = CoreWindow::GetMousePosition();
				m_line[0].position = m_edgeBeggining;
				m_line[1].position.x = static_cast<float>(mousePosition.x);
				m_line[1].position.y = static_cast<float>(mousePosition.y);
				m_line[1].position += CoreWindow::GetViewOffset();
				m_line[0].color = sf::Color::Red;
				m_line[1].color = m_line[0].color;
				CoreWindow::GetRenderWindow().draw(m_line.data(), 2, sf::Lines);
			}

			m_edgeSubmodeText.Draw();
			m_edgeCountText.Draw();
			break;
		}
		case ActiveMode::CAR:
		{
			m_carSubmodeText.Draw();
			m_carAngleText.Draw();
			break;
		}
		default:
			break;
	}

	m_activeModeText.Draw();
	m_movementText.Draw();
	m_viewOffsetXText.Draw();
	m_viewOffsetYText.Draw();
	m_filenameText.Draw();
}
