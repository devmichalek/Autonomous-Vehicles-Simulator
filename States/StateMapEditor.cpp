#include "StateMapEditor.hpp"
#include "CoreWindow.hpp"
#include "FilenameText.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionEventObserver.hpp"
#include "CoreLogger.hpp"

StateMapEditor::StateMapEditor() :
	m_pressedKeyTimer(0.0, 1.0, 5000),
	m_vehiclePrototype(nullptr),
	m_viewMovement(500.0, 1500.0, 50.0, 900.0),
	m_zoom(1.f, 4.f, 0.3f, 1.f)
{
	m_modeStrings[EDGE_MODE] = "Edge mode";
	m_modeStrings[VEHICLE_MODE] = "Vehicle mode";
	m_mode = EDGE_MODE;

	m_edgeSubmodeStrings[GLUED_INSERT_EDGE_SUBMODE] = "Glued insert mode";
	m_edgeSubmodeStrings[REMOVE_EDGE_SUBMODE] = "Remove mode";
	m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;

	m_controlKeys.insert(std::pair(sf::Keyboard::F1, CHANGE_TO_EDGE_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::F2, CHANGE_TO_VEHICLE_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Add, INCREASE_MOVEMENT));
	m_controlKeys.insert(std::pair(sf::Keyboard::Subtract, DECREASE_MOVEMENT));
	m_controlKeys.insert(std::pair(sf::Keyboard::Multiply, INCREASE_ZOOM));
	m_controlKeys.insert(std::pair(sf::Keyboard::Divide, DECREASE_ZOOM));
	m_controlKeys.insert(std::pair(sf::Keyboard::A, MOVE_OFFSET_LEFT));
	m_controlKeys.insert(std::pair(sf::Keyboard::D, MOVE_OFFSET_RIGHT));
	m_controlKeys.insert(std::pair(sf::Keyboard::W, MOVE_OFFSET_UP));
	m_controlKeys.insert(std::pair(sf::Keyboard::S, MOVE_OFFSET_DOWN));
	m_controlKeys.insert(std::pair(sf::Keyboard::Num1, CHANGE_TO_EDGE_MODE_INSERT_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Num2, CHANGE_TO_EDGE_MODE_REMOVE_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::LAlt, FIND_NEAREST_EDGE_POINT));
	m_controlKeys.insert(std::pair(sf::Keyboard::Escape, CANCEL_EDGE));
	m_controlKeys.insert(std::pair(sf::Keyboard::X, INCREASE_VEHICLE_ANGLE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Z, DECREASE_VEHICLE_ANGLE));
	m_controlKeys.insert(std::pair(sf::Keyboard::BackSpace, REMOVE_VEHICLE));

	for (auto& i : m_pressedKeys)
		i = false;

	// Initialize timer
	m_pressedKeyTimer.MakeTimeout();

	m_insertEdge = false;
	m_removeEdge = false;
	m_upToDate = false;
	m_vehiclePositioned = false;

	auto allowedMapArea = m_mapBuilder.GetMaxAllowedMapArea();
	auto allowedViewArea = m_mapBuilder.GetMaxAllowedViewArea();
	m_allowedMapAreaShape.setFillColor(ColorContext::ClearBackground);
	m_allowedMapAreaShape.setOutlineColor(ColorContext::Create(ColorContext::ClearBackground, ColorContext::MaxChannelValue / 4));
	m_allowedMapAreaShape.setOutlineThickness(5);
	m_allowedMapAreaShape.setPosition(allowedMapArea.first);
	m_allowedMapAreaShape.setSize(allowedMapArea.second);
	m_allowedViewAreaShape.setPosition(allowedViewArea.first);
	m_allowedViewAreaShape.setSize(allowedViewArea.second);
	
	m_vehicleBuilder.CreateDummy();
	m_vehiclePrototype = m_vehicleBuilder.Get();

	m_mapBuilder.CreateDummy();
	m_mapPrototype.SetEdgesChains(m_mapBuilder.GetInnerEdgesChain(), m_mapBuilder.GetOuterEdgesChain());
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
	for (auto& i : m_pressedKeys)
		i = false;
	m_pressedKeyTimer.MakeTimeout();
	m_insertEdge = false;
	m_removeEdge = false;
	m_edgeBeggining = sf::Vector2f(0.0, 0.0);
	m_upToDate = false;
	delete m_vehiclePrototype;
	m_vehiclePrototype = m_vehicleBuilder.Get();
	m_viewMovement.ResetValue();
	m_zoom.ResetValue();

	m_mapBuilder.CreateDummy();
	m_mapPrototype.SetEdgesChains(m_mapBuilder.GetInnerEdgesChain(), m_mapBuilder.GetOuterEdgesChain());
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
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	filenameText->Capture();
	if (!filenameText->IsRenaming())
	{
		if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
			{
				switch (iterator->second)
				{
					case CHANGE_TO_EDGE_MODE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == VEHICLE_MODE)
						{
							m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;
							m_textObservers[EDGE_SUBMODE_TEXT]->Notify();
							m_insertEdge = false;
							m_removeEdge = false;
							m_mode = EDGE_MODE;
							m_textObservers[MODE_TEXT]->Notify();
						}
						break;
					case CHANGE_TO_VEHICLE_MODE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == EDGE_MODE)
						{
							m_mode = VEHICLE_MODE;
							m_textObservers[MODE_TEXT]->Notify();
							m_textObservers[VEHICLE_POSITIONED_TEXT]->Notify();
							m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
						}
						break;
					case INCREASE_MOVEMENT:
						if (m_pressedKeyTimer.Update())
						{
							m_viewMovement.Increase();
							m_textObservers[MOVEMENT_TEXT]->Notify();
						}
						break;
					case DECREASE_MOVEMENT:
						if (m_pressedKeyTimer.Update())
						{
							m_viewMovement.Decrease();
							m_textObservers[MOVEMENT_TEXT]->Notify();
						}
						break;
					case INCREASE_ZOOM:
						if (m_pressedKeyTimer.Update())
						{
							m_zoom.Increase();
							CoreWindow::SetViewZoom(m_zoom);
							m_textObservers[ZOOM_TEXT]->Notify();

							if (m_allowedViewAreaShape.getSize().x < CoreWindow::GetViewSize().x ||
								m_allowedViewAreaShape.getSize().y < CoreWindow::GetViewSize().y)
							{
								auto center = m_allowedViewAreaShape.getPosition();
								center += m_allowedViewAreaShape.getSize() / 2.f;
								CoreWindow::SetViewCenter(center);
							}
						}
						break;
					case DECREASE_ZOOM:
						if (m_pressedKeyTimer.Update())
						{
							m_zoom.Decrease();
							CoreWindow::SetViewZoom(m_zoom);
							m_textObservers[ZOOM_TEXT]->Notify();
						}
						break;
					case MOVE_OFFSET_LEFT:
					case MOVE_OFFSET_RIGHT:
					case MOVE_OFFSET_UP:
					case MOVE_OFFSET_DOWN:
						m_pressedKeys[iterator->second] = true;
						break;
					case CHANGE_TO_EDGE_MODE_INSERT_STATE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == EDGE_MODE && m_edgeSubmode != GLUED_INSERT_EDGE_SUBMODE)
						{
							m_edgeSubmode = GLUED_INSERT_EDGE_SUBMODE;
							m_insertEdge = false;
							m_removeEdge = false;
							m_textObservers[EDGE_SUBMODE_TEXT]->Notify();
						}
						break;
					case CHANGE_TO_EDGE_MODE_REMOVE_STATE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == EDGE_MODE && m_edgeSubmode != REMOVE_EDGE_SUBMODE)
						{
							m_edgeSubmode = REMOVE_EDGE_SUBMODE;
							m_insertEdge = false;
							m_removeEdge = false;
							m_textObservers[EDGE_SUBMODE_TEXT]->Notify();
						}
						break;
					case FIND_NEAREST_EDGE_POINT:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == EDGE_MODE && m_edgeSubmode == GLUED_INSERT_EDGE_SUBMODE)
						{
							sf::Vector2f correctPosition = CoreWindow::GetMousePosition();
							correctPosition.x *= m_zoom;
							correctPosition.y *= m_zoom;
							correctPosition += CoreWindow::GetViewOffset();
							m_edgeBeggining = correctPosition;

							if (m_mapPrototype.FindClosestPointOnDistance(correctPosition, m_edgeBeggining))
								m_insertEdge = true;
						}
						break;
					case CANCEL_EDGE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == EDGE_MODE)
						{
							m_insertEdge = false;
							m_removeEdge = false;
						}
						break;
					case INCREASE_VEHICLE_ANGLE:
						if (m_pressedKeyTimer.Update())
						{
							if (m_mode == VEHICLE_MODE && m_vehiclePositioned)
							{
								auto angle = m_vehiclePrototype->GetAngle();
								angle += MapBuilder::GetVehicleAngleOffset();
								if (angle > MapBuilder::GetMaxVehicleAngle())
									angle = MapBuilder::GetMinVehicleAngle();
								m_vehiclePrototype->SetAngle(angle);
								m_vehiclePrototype->Update();
								m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
							}
						}
						break;
					case DECREASE_VEHICLE_ANGLE:
						if (m_pressedKeyTimer.Update())
						{
							if (m_mode == VEHICLE_MODE && m_vehiclePositioned)
							{
								auto angle = m_vehiclePrototype->GetAngle();
								angle -= MapBuilder::GetVehicleAngleOffset();
								if (angle < MapBuilder::GetMinVehicleAngle())
									angle = MapBuilder::GetMaxVehicleAngle();
								m_vehiclePrototype->SetAngle(angle);
								m_vehiclePrototype->Update();
								m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
							}
						}
						break;
					case REMOVE_VEHICLE:
						m_pressedKeys[iterator->second] = true;
						if (m_mode == VEHICLE_MODE && m_vehiclePositioned)
						{
							m_vehiclePositioned = false;
							m_vehiclePrototype->SetAngle(0.0);
							m_vehiclePrototype->Update();
							m_textObservers[VEHICLE_POSITIONED_TEXT]->Notify();
							m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
						}
						break;
				}
			}
		}
		else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end())
			{
				m_pressedKeys[iterator->second] = false;
				m_pressedKeyTimer.MakeTimeout();
			}
		}
		else if (CoreWindow::GetEvent().type == sf::Event::MouseButtonPressed)
		{
			sf::Vector2f correctPosition = CoreWindow::GetMousePosition();
			correctPosition.x *= m_zoom;
			correctPosition.y *= m_zoom;
			correctPosition += CoreWindow::GetViewOffset();
			if (MathContext::IsPointInsideRectangle(m_allowedMapAreaShape.getSize(), m_allowedMapAreaShape.getPosition(), correctPosition))
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
	}
	else
		m_upToDate = false;
}

void StateMapEditor::Update()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsReading())
	{
		bool success = m_mapBuilder.Load(filenameText->GetFilename());
		auto status = m_mapBuilder.GetLastOperationStatus();
		if (success)
		{
			filenameText->SetSuccessStatusText(status.second);
			m_mapPrototype.SetEdgesChains(m_mapBuilder.GetInnerEdgesChain(), m_mapBuilder.GetOuterEdgesChain());
			m_vehiclePositioned = true;
			m_vehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
			m_vehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
			m_vehiclePrototype->Update();
			m_textObservers[VEHICLE_ANGLE_TEXT]->Notify();
			m_textObservers[NUMBER_OF_INNER_EDGES_TEXT]->Notify();
			m_textObservers[NUMBER_OF_OUTER_EDGES_TEXT]->Notify();
			m_textObservers[INNER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
			m_textObservers[OUTER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
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
			m_mapBuilder.AddEdgesChains(m_mapPrototype.GetInnerEdgesChain(), m_mapPrototype.GetOuterEdgesChain());
			bool success = m_mapBuilder.Save(filenameText->GetFilename());
			auto status = m_mapBuilder.GetLastOperationStatus();
			if (success)
				filenameText->SetSuccessStatusText(status.second);
			else
				filenameText->SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (!filenameText->IsRenaming())
	{
		bool moveHorizontally = m_allowedViewAreaShape.getSize().x > CoreWindow::GetViewSize().x;
		bool moveVertically = m_allowedViewAreaShape.getSize().y > CoreWindow::GetViewSize().y;
		float elapsedTime = float(CoreWindow::GetElapsedTime());
		auto& view = CoreWindow::GetView();
		float moveOffset = static_cast<float>(m_viewMovement * elapsedTime);

		if (moveHorizontally)
		{
			if (m_pressedKeys[MOVE_OFFSET_LEFT])
			{
				view.move(sf::Vector2f(-moveOffset, 0));
				m_textObservers[VIEW_OFFSET_TEXT]->Notify();
			}
			else if (m_pressedKeys[MOVE_OFFSET_RIGHT])
			{
				view.move(sf::Vector2f(moveOffset, 0));
				m_textObservers[VIEW_OFFSET_TEXT]->Notify();
			}

			auto viewPosition = CoreWindow::GetViewOffset();
			float left = m_allowedViewAreaShape.getPosition().x;
			float right = m_allowedViewAreaShape.getPosition().x + m_allowedViewAreaShape.getSize().x;

			if (viewPosition.x < left)
			{
				auto difference = left - viewPosition.x;
				if (difference > 1.f)
				{
					view.move(sf::Vector2f(difference, 0));
					m_textObservers[VIEW_OFFSET_TEXT]->Notify();
				}
			}
			else if (viewPosition.x + CoreWindow::GetViewSize().x > right)
			{
				auto difference = (viewPosition.x + CoreWindow::GetViewSize().x - right);
				if (difference > 1.f)
				{
					view.move(sf::Vector2f(-difference, 0));
					m_textObservers[VIEW_OFFSET_TEXT]->Notify();
				}
			}
			CoreWindow::SetView(view);
		}

		if (moveVertically)
		{
			if (m_pressedKeys[MOVE_OFFSET_UP])
			{
				view.move(sf::Vector2f(0, -moveOffset));
				m_textObservers[VIEW_OFFSET_TEXT]->Notify();
			}
			else if (m_pressedKeys[MOVE_OFFSET_DOWN])
			{
				view.move(sf::Vector2f(0, moveOffset));
				m_textObservers[VIEW_OFFSET_TEXT]->Notify();
			}

			auto viewPosition = CoreWindow::GetViewOffset();
			float top = m_allowedViewAreaShape.getPosition().y;
			float bot = m_allowedViewAreaShape.getPosition().y + m_allowedViewAreaShape.getSize().y;

			if (viewPosition.y < top)
			{
				auto difference = top - viewPosition.y;
				if (difference > 1.f)
				{
					view.move(sf::Vector2f(0, difference));
					m_textObservers[VIEW_OFFSET_TEXT]->Notify();
				}
			}
			else if (viewPosition.y + CoreWindow::GetViewSize().y > bot)
			{
				auto difference = viewPosition.y + CoreWindow::GetViewSize().y - bot;
				if (difference > 1.f)
				{
					view.move(sf::Vector2f(0, -difference));
					m_textObservers[VIEW_OFFSET_TEXT]->Notify();
				}
			}
			CoreWindow::SetView(view);
		}
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
	m_texts[ZOOM_TEXT] = new TripleText({ "Zoom:", "", "| [/] [*]" });
	m_texts[VIEW_OFFSET_TEXT] = new TripleText({ "View offset:", "", "| [A] [D] [W] [S]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, true>("map.bin");
	m_texts[EDGE_SUBMODE_TEXT] = new TripleText({ "Current mode:", "", "| [1] [2] [RMB] [Alt] [Esc]" });
	m_texts[NUMBER_OF_INNER_EDGES_TEXT] = new DoubleText({ "Number of inner edges:" });
	m_texts[NUMBER_OF_OUTER_EDGES_TEXT] = new DoubleText({ "Number of outer edges:" });
	m_texts[INNER_EDGES_CHAIN_COMPLETED_TEXT] = new DoubleText({ "Inner edges chain completed:" });
	m_texts[OUTER_EDGES_CHAIN_COMPLETED_TEXT] = new DoubleText({ "Outer edges chain completed:" });
	m_texts[VEHICLE_POSITIONED_TEXT] = new TripleText({ "Vehicle positioned:", "", "| [RMB] [Backspace]" });
	m_texts[VEHICLE_ANGLE_TEXT] = new TripleText({ "Vehicle angle:", "", "| [Z] [X]" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[MOVEMENT_TEXT] = new FunctionEventObserver<size_t>([&] { return size_t(m_viewMovement); });
	m_textObservers[ZOOM_TEXT] = new FunctionEventObserver<float>([&] { return m_zoom; });
	m_textObservers[VIEW_OFFSET_TEXT] = new FunctionEventObserver<std::string>([&] { return "(" + std::to_string(int(CoreWindow::GetViewOffset().x)) + ", " + std::to_string(int(CoreWindow::GetViewOffset().y)) + ")"; });
	m_textObservers[FILENAME_TEXT] = nullptr;
	m_textObservers[EDGE_SUBMODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_edgeSubmodeStrings[m_edgeSubmode]; });
	m_textObservers[NUMBER_OF_INNER_EDGES_TEXT] = new FunctionEventObserver<size_t>([&] { return m_mapPrototype.GetNumberOfInnerEdges(); });
	m_textObservers[NUMBER_OF_OUTER_EDGES_TEXT] = new FunctionEventObserver<size_t>([&] { return m_mapPrototype.GetNumberOfOuterEdges(); });
	m_textObservers[INNER_EDGES_CHAIN_COMPLETED_TEXT] = new FunctionEventObserver<bool>([&] { return m_mapPrototype.IsInnerEdgesChainCompleted(); });
	m_textObservers[OUTER_EDGES_CHAIN_COMPLETED_TEXT] = new FunctionEventObserver<bool>([&] { return m_mapPrototype.IsOuterEdgesChainCompleted(); });
	m_textObservers[VEHICLE_POSITIONED_TEXT] = new TypeEventObserver<bool>(m_vehiclePositioned);
	m_textObservers[VEHICLE_ANGLE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehiclePositioned ? std::to_string(m_vehiclePrototype->GetAngle()) : "Unknown"; });

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		((DoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);
	
	// Set text positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {7} });
	m_texts[MOVEMENT_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_texts[ZOOM_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {7} });
	m_texts[VIEW_OFFSET_TEXT]->SetPosition({ FontContext::Component(3), {0}, {3}, {7} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(4), {0}, {3}, {7}, {16} });
	m_texts[EDGE_SUBMODE_TEXT]->SetPosition({ FontContext::Component(5, true), {0}, {6}, {10} });
	m_texts[NUMBER_OF_INNER_EDGES_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {6} });
	m_texts[NUMBER_OF_OUTER_EDGES_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {6} });
	m_texts[INNER_EDGES_CHAIN_COMPLETED_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {6} });
	m_texts[OUTER_EDGES_CHAIN_COMPLETED_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {6} });
	m_texts[VEHICLE_POSITIONED_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {4}, {7} });
	m_texts[VEHICLE_ANGLE_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {4}, {7} });

	auto tmp = CoreWindow::GetViewOffset();
	CoreLogger::PrintSuccess("StateMapEditor dependencies loaded correctly");
	return true;
}

void StateMapEditor::Draw()
{
	if (m_vehiclePositioned)
	{
		m_vehiclePrototype->DrawBody();
		m_vehiclePrototype->DrawBeams();
		m_vehiclePrototype->DrawSensors();
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
				edgeShape[1].position = CoreWindow::GetMousePosition();
				edgeShape[1].position.x *= m_zoom;
				edgeShape[1].position.y *= m_zoom;
				edgeShape[1].position += CoreWindow::GetViewOffset();
				edgeShape[0].color = ColorContext::EdgeDefault;
				edgeShape[1].color = edgeShape[0].color;
				CoreWindow::Draw(edgeShape.data(), edgeShape.size(), sf::Lines);
			}
			else if (m_removeEdge)
			{
				EdgeShape edgeShape;
				edgeShape[0].position = m_edgeBeggining;
				edgeShape[1].position = CoreWindow::GetMousePosition();
				edgeShape[1].position.x *= m_zoom;
				edgeShape[1].position.y *= m_zoom;
				edgeShape[1].position += CoreWindow::GetViewOffset();
				edgeShape[0].color = ColorContext::EdgeRemove;
				edgeShape[1].color = edgeShape[0].color;
				CoreWindow::Draw(edgeShape.data(), edgeShape.size(), sf::Lines);
			}

			m_texts[EDGE_SUBMODE_TEXT]->Draw();
			m_texts[NUMBER_OF_INNER_EDGES_TEXT]->Draw();
			m_texts[NUMBER_OF_OUTER_EDGES_TEXT]->Draw();
			m_texts[INNER_EDGES_CHAIN_COMPLETED_TEXT]->Draw();
			m_texts[OUTER_EDGES_CHAIN_COMPLETED_TEXT]->Draw();
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
	m_texts[ZOOM_TEXT]->Draw();
	m_texts[VIEW_OFFSET_TEXT]->Draw();
	m_texts[FILENAME_TEXT]->Draw();
}

void StateMapEditor::InsertEdge(sf::Vector2f edgeEndPoint)
{
	if (m_insertEdge)
	{
		if (!m_mapPrototype.IsInnerEdgesChainCompleted())
		{
			m_insertEdge = !m_mapPrototype.FindClosestPointOnIntersection({ m_edgeBeggining, edgeEndPoint }, edgeEndPoint);
			if (m_mapPrototype.AddInnerEdge({ m_edgeBeggining, edgeEndPoint }))
			{
				m_textObservers[NUMBER_OF_INNER_EDGES_TEXT]->Notify();
				m_textObservers[INNER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
				m_upToDate = false;
			}
		}
		else if (!m_mapPrototype.IsOuterEdgesChainCompleted())
		{
			m_insertEdge = !m_mapPrototype.FindClosestPointOnIntersection({ m_edgeBeggining, edgeEndPoint }, edgeEndPoint);
			if (m_mapPrototype.AddOuterEdge({ m_edgeBeggining, edgeEndPoint }))
			{
				m_textObservers[NUMBER_OF_OUTER_EDGES_TEXT]->Notify();
				m_textObservers[OUTER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
				m_upToDate = false;
			}
		}
	}
	else
		m_insertEdge = true;

	m_edgeBeggining = edgeEndPoint;
}

void StateMapEditor::RemoveEdge(sf::Vector2f edgeEndPoint)
{
	if (m_removeEdge)
	{
		if (m_mapPrototype.RemoveEdgesOnIntersection({ m_edgeBeggining, edgeEndPoint }))
		{
			if (!m_mapPrototype.IsInnerEdgesChainCompleted())
			{
				m_textObservers[NUMBER_OF_INNER_EDGES_TEXT]->Notify();
				m_textObservers[INNER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
			}

			m_textObservers[NUMBER_OF_OUTER_EDGES_TEXT]->Notify();
			m_textObservers[OUTER_EDGES_CHAIN_COMPLETED_TEXT]->Notify();
			m_upToDate = false;
		}
	}
	else
		m_edgeBeggining = edgeEndPoint;

	m_removeEdge = !m_removeEdge;
}
