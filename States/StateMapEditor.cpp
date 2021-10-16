#include "StateMapEditor.hpp"
#include "CoreWindow.hpp"
#include "FilenameText.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionEventObserver.hpp"
#include "CoreLogger.hpp"
#include <functional>

StateMapEditor::StateMapEditor() :
	m_vehiclePrototype(nullptr),
	m_viewMovementTimer(0.0, 0.1),
	m_viewMovement(500.0, 1500.0, 50.0, 900.0)
{
	m_modeStrings[EDGE_MODE] = "Edge mode";
	m_modeStrings[VEHICLE_MODE] = "Vehicle mode";
	m_mode = EDGE_MODE;

	m_edgeSubmodeStrings[GLUED_INSERT_EDGE_SUBMODE] = "Glued insert mode";
	m_edgeSubmodeStrings[REMOVE_EDGE_SUBMODE] = "Remove mode";
	m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;

	m_insertEdge = false;
	m_removeEdge = false;
	m_upToDate = false;
	m_vehiclePositioned = false;

	auto allowedMapArea = m_mapBuilder.GetMaxAllowedMapArea();
	auto allowedViewArea = m_mapBuilder.GetMaxAllowedViewArea();
	m_allowedMapAreaShape.setFillColor(sf::Color(255, 255, 255, 0));
	m_allowedMapAreaShape.setOutlineColor(sf::Color(255, 255, 255, 64));
	m_allowedMapAreaShape.setOutlineThickness(2);
	m_allowedMapAreaShape.setPosition(allowedMapArea.first);
	m_allowedMapAreaShape.setSize(allowedMapArea.second);
	m_allowedViewAreaShape.setPosition(allowedViewArea.first);
	m_allowedViewAreaShape.setSize(allowedViewArea.second);
	
	m_vehicleBuilder.CreateDummy();
	m_vehiclePrototype = m_vehicleBuilder.Get();

	m_mapBuilder.CreateDummy();
	m_mapPrototype.SetEdges(m_mapBuilder.GetEdges());
	m_vehiclePositioned = true;
	m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
	m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
	m_vehiclePrototype->Update();

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateMapEditor::~StateMapEditor()
{
	delete m_vehiclePrototype;
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateMapEditor::Reload()
{
	// Reset internal states
	m_mode = EDGE_MODE;
	m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;
	m_insertEdge = false;
	m_removeEdge = false;
	m_edgeBeggining = sf::Vector2f(0.0, 0.0);
	m_upToDate = false;
	delete m_vehiclePrototype;
	m_vehiclePrototype = m_vehicleBuilder.Get();

	// Reset view movement
	m_viewMovementTimer.Reset();
	m_viewMovement.ResetValue();

	m_mapBuilder.CreateDummy();
	m_mapPrototype.SetEdges(m_mapBuilder.GetEdges());
	m_vehiclePositioned = true;
	m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
	m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
	m_vehiclePrototype->Update();

	CoreWindow::Reset();

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}
}

void StateMapEditor::Capture()
{
	if (CoreWindow::GetEvent().type == sf::Event::MouseButtonPressed)
	{
		sf::Vector2f correctPosition = CoreWindow::GetMousePosition() + CoreWindow::GetViewOffset();
		if (DrawableMath::IsPointInsideRectangle(m_allowedMapAreaShape.getSize(), m_allowedMapAreaShape.getPosition(), correctPosition))
		{
			switch (m_mode)
			{
				case EDGE_MODE:
				{
					switch (m_edgeSubmode)
					{
						case GLUED_INSERT_EDGE_SUBMODE:
							InsertEdge(correctPosition);
							break;
						case REMOVE_EDGE_SUBMODE:
							RemoveEdge(correctPosition);
							break;
					}
					break;
				}
				case VEHICLE_MODE:
				{
					m_vehiclePrototype->SetCenter(correctPosition);
					m_vehiclePrototype->Update();

					if (!m_vehiclePositioned)
					{
						m_vehiclePositioned = true;
						// If vehicle was not previously positioned notify observers
						m_textObservers[VEHICLE_POSITIONED_TEXT]->Notify();
						m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
					}
					
					m_upToDate = false;
					break;
				}
				default:
					break;
			}
		}
	}

	static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT])->Capture();
}

void StateMapEditor::Update()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsReading())
	{
		bool success = m_mapBuilder.Load(filenameText->GetFilename());
		auto status = m_mapBuilder.GetLastOperationStatus();
		filenameText->ShowStatusText();
		if (success)
		{
			filenameText->SetSuccessStatusText(status.second);
			m_mapPrototype.SetEdges(m_mapBuilder.GetEdges());
			m_textObservers[EDGE_COUNT_TEXT]->Notify();
			m_vehiclePositioned = true;
			m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
			m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
			m_vehiclePrototype->Update();
			m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
			m_upToDate = true;
		}
		else
			filenameText->SetErrorStatusText(status.second);
	}
	else if (filenameText->IsWriting())
	{
		if (!m_upToDate)
		{
			m_mapBuilder.Clear();
			if (m_vehiclePositioned)
				m_mapBuilder.AddVehicle(m_vehiclePrototype->GetAngle(), m_vehiclePrototype->GetCenter());
			m_mapBuilder.AddEdges(m_mapPrototype.GetEdges());
			bool success = m_mapBuilder.Save(filenameText->GetFilename());
			auto status = m_mapBuilder.GetLastOperationStatus();
			filenameText->ShowStatusText();
			if (success)
				filenameText->SetSuccessStatusText(status.second);
			else
				filenameText->SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (!filenameText->IsRenaming())
	{
		switch (m_mode)
		{
			case EDGE_MODE:
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
				{
					m_mode = VEHICLE_MODE;
					m_textObservers[MODE_TEXT]->Notify();
					m_textObservers[VEHICLE_POSITIONED_TEXT]->Notify();
					m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt))
				{
					if (!m_mapPrototype.IsEmpty() && m_edgeSubmode == GLUED_INSERT_EDGE_SUBMODE)
					{
						sf::Vector2f correctPosition = CoreWindow::GetMousePosition() + CoreWindow::GetViewOffset();
						m_edgeBeggining = correctPosition;
						m_insertEdge = true;

						// Find closest point to mouse position
						m_mapPrototype.FindClosestPointOnDistance(correctPosition, m_edgeBeggining);
					}
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					m_insertEdge = false;
					m_removeEdge = false;
				}
				else
				{
					size_t mode = m_edgeSubmode;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
					{
						mode = GLUED_INSERT_EDGE_SUBMODE;
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) ||
						sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
					{
						mode = REMOVE_EDGE_SUBMODE;
					}

					if (m_edgeSubmode != mode)
					{
						m_edgeSubmode = mode;
						m_insertEdge = false;
						m_removeEdge = false;
						m_textObservers[EDGE_SUBMODE_TEXT]->Notify();
					}
				}

				break;
			}
			case VEHICLE_MODE:
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
				{
					m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;
					m_textObservers[EDGE_SUBMODE_TEXT]->Notify();
					m_insertEdge = false;
					m_removeEdge = false;
					m_mode = EDGE_MODE;
					m_textObservers[MODE_TEXT]->Notify();
				}
				else
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
					{
						if (m_vehiclePositioned)
						{
							m_vehiclePositioned = false;
							m_vehiclePrototype->SetAngle(0.0);
							m_vehiclePrototype->Update();
							m_textObservers[VEHICLE_POSITIONED_TEXT]->Notify();
							m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
						}
					}
					else if (m_vehiclePositioned)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
						{
							auto angle = m_vehiclePrototype->GetAngle();
							angle -= 150.0 * CoreWindow::GetElapsedTime();
							if (angle < MapBuilder::GetMinVehicleAngle())
								angle = MapBuilder::GetMaxVehicleAngle();
							m_vehiclePrototype->SetAngle(angle);
							m_vehiclePrototype->Update();
							m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
						{
							auto angle = m_vehiclePrototype->GetAngle();
							angle += 150.0 * CoreWindow::GetElapsedTime();
							if (angle > MapBuilder::GetMaxVehicleAngle())
								angle = MapBuilder::GetMinVehicleAngle();
							m_vehiclePrototype->SetAngle(angle);
							m_vehiclePrototype->Update();
							m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
						}
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
			if (m_viewMovementTimer.Update())
			{
				m_viewMovement.Increase();
				m_textObservers[MOVEMENT_TEXT]->Notify();
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
		{
			if (m_viewMovementTimer.Update())
			{
				m_viewMovement.Decrease();
				m_textObservers[MOVEMENT_TEXT]->Notify();
			}
		}

		auto& view = CoreWindow::GetView();
		auto viewPosition = view.getCenter() - (CoreWindow::GetWindowSize() / 2.0f);
		float moveOffset = static_cast<float>(m_viewMovement * elapsedTime);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			view.move(sf::Vector2f(-moveOffset, 0));
			m_textObservers[VIEW_OFFSET_X_TEXT]->Notify();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			view.move(sf::Vector2f(moveOffset, 0));
			m_textObservers[VIEW_OFFSET_X_TEXT]->Notify();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			view.move(sf::Vector2f(0, -moveOffset));
			m_textObservers[VIEW_OFFSET_Y_TEXT]->Notify();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			view.move(sf::Vector2f(0, moveOffset));
			m_textObservers[VIEW_OFFSET_Y_TEXT]->Notify();
		}

		view = CoreWindow::GetView();
		viewPosition = view.getCenter() - (CoreWindow::GetWindowSize() / 2.0f);
		float left = m_allowedViewAreaShape.getPosition().x;
		float right = m_allowedViewAreaShape.getPosition().x + m_allowedViewAreaShape.getSize().x;
		float top = m_allowedViewAreaShape.getPosition().y;
		float bot = m_allowedViewAreaShape.getPosition().y + m_allowedViewAreaShape.getSize().y;

		if (viewPosition.x < left)
			view.move(sf::Vector2f(left - viewPosition.x, 0));
		else if (viewPosition.x + CoreWindow::GetWindowSize().x > right)
			view.move(sf::Vector2f(-(viewPosition.x + CoreWindow::GetWindowSize().x - right), 0));

		if (viewPosition.y < top)
			view.move(sf::Vector2f(0, top - viewPosition.y));
		else if (viewPosition.y + CoreWindow::GetWindowSize().y > bot)
			view.move(sf::Vector2f(0, -(viewPosition.y + CoreWindow::GetWindowSize().y - bot)));

		CoreWindow::SetView(view);
	}
	else
		m_upToDate = false;

	for (const auto& text : m_texts)
		text->Update();
}

bool StateMapEditor::Load()
{
	// Create texts
	m_texts[MODE_TEXT] = new TripleText({ "Mode:", "",  "| [F1] [F2]" });
	m_texts[MOVEMENT_TEXT] = new TripleText({ "Movement:", "", "| [+] [-]" });
	m_texts[VIEW_OFFSET_X_TEXT] = new TripleText({ "View offset x:", "", "| [A] [D]" });
	m_texts[VIEW_OFFSET_Y_TEXT] = new TripleText({ "View offset y:", "", "| [W] [S]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, true>("map.bin");
	m_texts[EDGE_SUBMODE_TEXT] = new TripleText({ "Current mode:", "", "| [1] [2] [RMB] [Alt] [Esc]" });
	m_texts[EDGE_COUNT_TEXT] = new DoubleText({ "Edge count:" });
	m_texts[VEHICLE_POSITIONED_TEXT] = new TripleText({ "Vehicle positioned:", "", "| [RMB] [Backspace]" });
	m_texts[VEHICLE_ANGLE_TEXT] = new TripleText({ "Vehicle angle:", "", "| [Z] [X]" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[MOVEMENT_TEXT] = new FunctionEventObserver<size_t>([&] { return size_t(m_viewMovement); });
	m_textObservers[VIEW_OFFSET_X_TEXT] = new FunctionEventObserver<int>([&] { return int(CoreWindow::GetViewOffset().x); });
	m_textObservers[VIEW_OFFSET_Y_TEXT] = new FunctionEventObserver<int>([&] { return int(CoreWindow::GetViewOffset().y); });
	m_textObservers[FILENAME_TEXT] = nullptr;
	m_textObservers[EDGE_SUBMODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_edgeSubmodeStrings[m_edgeSubmode]; });
	m_textObservers[EDGE_COUNT_TEXT] = new FunctionEventObserver<size_t>([&] { return m_mapPrototype.GetNumberOfEdges(); });
	m_textObservers[VEHICLE_POSITIONED_TEXT] = new TypeEventObserver<bool>(m_vehiclePositioned);
	m_textObservers[VEHICLE_ANGLE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehiclePositioned ? std::to_string(m_vehiclePrototype->GetAngle()) : "Unknown"; });

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);
	
	// Set text positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {7} });
	m_texts[MOVEMENT_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_texts[VIEW_OFFSET_X_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {7} });
	m_texts[VIEW_OFFSET_Y_TEXT]->SetPosition({ FontContext::Component(3), {0}, {3}, {7} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(4), {0}, {3}, {7}, {16} });
	m_texts[EDGE_SUBMODE_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {3}, {7} });
	m_texts[EDGE_COUNT_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {3} });
	m_texts[VEHICLE_POSITIONED_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {4}, {7} });
	m_texts[VEHICLE_ANGLE_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {4}, {7} });

	CoreLogger::PrintSuccess("State \"Map Editor\" dependencies loaded correctly");
	return true;
}

void StateMapEditor::Draw()
{
	if (m_vehiclePositioned)
	{
		m_vehiclePrototype->DrawBody();
		m_vehiclePrototype->DrawBeams();
	}

	m_mapPrototype.DrawEdges();

	CoreWindow::Draw(m_allowedMapAreaShape);

	switch (m_mode)
	{
		case EDGE_MODE:
		{
			if (m_insertEdge)
			{
				EdgeShape edgeShape;
				edgeShape[0].position = m_edgeBeggining;
				edgeShape[1].position = CoreWindow::GetMousePosition() + CoreWindow::GetViewOffset();
				edgeShape[0].color = sf::Color::White;
				edgeShape[1].color = edgeShape[0].color;
				CoreWindow::Draw(edgeShape.data(), edgeShape.size(), sf::Lines);
			}
			else if (m_removeEdge)
			{
				EdgeShape edgeShape;
				edgeShape[0].position = m_edgeBeggining;
				edgeShape[1].position = CoreWindow::GetMousePosition() + CoreWindow::GetViewOffset();
				edgeShape[0].color = sf::Color::Red;
				edgeShape[1].color = edgeShape[0].color;
				CoreWindow::Draw(edgeShape.data(), edgeShape.size(), sf::Lines);
			}

			m_texts[EDGE_SUBMODE_TEXT]->Draw();
			m_texts[EDGE_COUNT_TEXT]->Draw();
			break;
		}
		case VEHICLE_MODE:
		{
			m_texts[VEHICLE_POSITIONED_TEXT]->Draw();
			m_texts[VEHICLE_ANGLE_TEXT]->Draw();
			break;
		}
		default:
			break;
	}

	m_texts[MODE_TEXT]->Draw();
	m_texts[MOVEMENT_TEXT]->Draw();
	m_texts[VIEW_OFFSET_X_TEXT]->Draw();
	m_texts[VIEW_OFFSET_Y_TEXT]->Draw();
	m_texts[FILENAME_TEXT]->Draw();
}

void StateMapEditor::InsertEdge(sf::Vector2f edgeEndPoint)
{
	if (m_insertEdge)
	{
		if (!m_mapPrototype.IsEmpty())
			m_insertEdge = !m_mapPrototype.FindClosestPointOnIntersection({ m_edgeBeggining, edgeEndPoint }, edgeEndPoint);

		m_mapPrototype.AddEdge({ m_edgeBeggining, edgeEndPoint });
		m_textObservers[EDGE_COUNT_TEXT]->Notify();
		m_upToDate = false;
	}
	else
		m_insertEdge = true;

	m_edgeBeggining = edgeEndPoint;
}

void StateMapEditor::RemoveEdge(sf::Vector2f edgeEndPoint)
{
	if (m_removeEdge)
	{
		if (m_mapPrototype.RemoveEdgesOnInterscetion({ m_edgeBeggining, edgeEndPoint }))
		{
			m_upToDate = false;
			m_textObservers[EDGE_COUNT_TEXT]->Notify();
		}
	}
	else
		m_edgeBeggining = edgeEndPoint;

	m_removeEdge = !m_removeEdge;
}
