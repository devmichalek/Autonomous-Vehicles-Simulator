#include "StateVehicleEditor.hpp"
#include "FunctionTimerObserver.hpp"
#include "DrawableVariableText.hpp"
#include "DrawableFilenameText.hpp"
#include "CoreLogger.hpp"
#include <functional>

StateVehicleEditor::StateVehicleEditor()
{
	m_modeStrings[MODE_VEHICLE_BODY] = "Vehicle body";
	m_modeStrings[MODE_VEHICLE_SENSORS] = "Vehicle sensors";
	m_mode = MODE_VEHICLE_BODY;
	m_submodeStrings[INSERT] = "Inserting";
	m_submodeStrings[REMOVE] = "Removing";
	m_submode = INSERT;

	m_controlKeys[sf::Keyboard::F1] = CHANGE_TO_VEHICLE_BODY_MODE;
	m_controlKeys[sf::Keyboard::F2] = CHANGE_TO_VEHICLE_SENSORS_MODE;
	m_controlKeys[sf::Keyboard::Num1] = CHANGE_TO_INSERT_STATE;
	m_controlKeys[sf::Keyboard::Numpad1] = CHANGE_TO_INSERT_STATE;
	m_controlKeys[sf::Keyboard::Num2] = CHANGE_TO_REMOVE_STATE;
	m_controlKeys[sf::Keyboard::Numpad2] = CHANGE_TO_REMOVE_STATE;
	m_controlKeys[sf::Keyboard::RAlt] = CHANGE_SENSOR;
	m_controlKeys[sf::Keyboard::LAlt] = CHANGE_SENSOR;
	m_controlKeys[sf::Keyboard::Z] = DEACREASE_SENSOR_ANGLE;
	m_controlKeys[sf::Keyboard::X] = INCREASE_SENSOR_ANGLE;

	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	auto maxVehicleSize = DrawableVehicleBuilder::GetMaxVehicleBodySize();
	auto windowSize = CoreWindow::GetSize();
	m_allowedAreaShape.setFillColor(sf::Color(255, 255, 255, 0));
	m_allowedAreaShape.setOutlineColor(sf::Color(0, 0, 255, 64));
	m_allowedAreaShape.setOutlineThickness(2);
	m_allowedAreaShape.setSize(maxVehicleSize);
	m_allowedAreaShape.setPosition((windowSize.x / 2.0f - maxVehicleSize.x / 2.0f), (windowSize.y / 2.0f - maxVehicleSize.y / 2.0f));
	
	m_drawableVehicleBuilder.CreateDummy();
	m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
	m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();

	m_currentSensorIndex = m_vehicleSensors.GetNumberOfSensors() - 1;
	m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
	m_upToDate = false;

	// Set axes
	auto windowCenter = CoreWindow::GetCenter();
	m_yAxis[0].color = m_yAxis[1].color = m_xAxis[0].color = m_xAxis[1].color = sf::Color(0, 0, 255, 64);
	m_xAxis[0].position = sf::Vector2f(windowCenter.x, 0);
	m_xAxis[1].position = sf::Vector2f(windowCenter.x, windowSize.y);
	m_yAxis[0].position = sf::Vector2f(0, windowCenter.y);
	m_yAxis[1].position = sf::Vector2f(windowSize.x, windowCenter.y);

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateVehicleEditor::~StateVehicleEditor()
{
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateVehicleEditor::Reload()
{
	m_mode = MODE_VEHICLE_BODY;
	m_submode = INSERT;

	// Reset pressed keys
	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset subjects of change
	m_drawableVehicleBuilder.CreateDummy();
	m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
	m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();

	m_currentSensorIndex = m_vehicleSensors.GetNumberOfSensors() - 1;
	m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
	m_upToDate = false;

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateVehicleEditor::Capture()
{
	auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	filenameText->Capture();
	if (!filenameText->IsRenaming())
	{
		if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
			{
				m_pressedKeys[iterator->second] = true;
				switch (iterator->second)
				{
					case CHANGE_TO_VEHICLE_BODY_MODE:
						m_mode = MODE_VEHICLE_BODY;
						m_submode = INSERT;
						m_currentSensorIndex = 0;
						m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
						break;

					case CHANGE_TO_VEHICLE_SENSORS_MODE:
						m_mode = MODE_VEHICLE_SENSORS;
						m_submode = INSERT;
						m_currentSensorIndex = 0;
						m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
						break;

					case CHANGE_TO_INSERT_STATE:
						m_submode = INSERT;
						break;

					case CHANGE_TO_REMOVE_STATE:
						m_submode = REMOVE;
						break;

					case CHANGE_SENSOR:
						++m_currentSensorIndex;
						if (m_currentSensorIndex >= m_vehicleSensors.GetNumberOfSensors())
							m_currentSensorIndex = 0;
						m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
						m_upToDate = false;
						break;

					case DEACREASE_SENSOR_ANGLE:
						if (m_vehicleSensors.GetNumberOfSensors() > 0)
						{
							m_currentSensorAngle -= DrawableVehicleBuilder::GetDefaultSensorAngleOffset();
							if (m_currentSensorAngle < DrawableVehicleBuilder::GetMinSensorAngle())
								m_currentSensorAngle = DrawableVehicleBuilder::GetMaxSensorAngle() - DrawableVehicleBuilder::GetDefaultSensorAngleOffset();
							m_vehicleSensors.SetSensorAngle(m_currentSensorIndex, m_currentSensorAngle);
							m_upToDate = false;
						}
						break;

					case INCREASE_SENSOR_ANGLE:
						if (m_vehicleSensors.GetNumberOfSensors() > 0)
						{
							m_currentSensorAngle += DrawableVehicleBuilder::GetDefaultSensorAngleOffset();
							if (m_currentSensorAngle > DrawableVehicleBuilder::GetMaxSensorAngle())
								m_currentSensorAngle = DrawableVehicleBuilder::GetMinSensorAngle() + DrawableVehicleBuilder::GetDefaultSensorAngleOffset();
							else if (m_currentSensorAngle == DrawableVehicleBuilder::GetMaxSensorAngle())
								m_currentSensorAngle = DrawableVehicleBuilder::GetMinSensorAngle();
							m_vehicleSensors.SetSensorAngle(m_currentSensorIndex, m_currentSensorAngle);
							m_upToDate = false;
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
				m_pressedKeys[iterator->second] = false;
		}
		else if (CoreWindow::GetEvent().type == sf::Event::MouseButtonPressed)
		{
			auto relativePosition = CoreWindow::GetMousePosition();
			if (DrawableMath::IsPointInsideRectangle(m_allowedAreaShape.getSize(), m_allowedAreaShape.getPosition(), relativePosition))
			{
				relativePosition -= CoreWindow::GetCenter();
				switch (m_mode)
				{
					case MODE_VEHICLE_BODY:
					{
						switch (m_submode)
						{
							case INSERT:
								m_vehicleBody.AddPoint(relativePosition);
								m_upToDate = false;
								break;

							case REMOVE:
								m_vehicleBody.RemovePoint(relativePosition);
								m_upToDate = false;
								break;
						}

						break;
					}
					case MODE_VEHICLE_SENSORS:
					{
						switch (m_submode)
						{
							case INSERT:
								m_vehicleSensors.AddSensor(relativePosition, DrawableVehicleBuilder::GetMinSensorAngle());
								m_currentSensorIndex = m_vehicleSensors.GetNumberOfSensors() - 1;
								m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
								m_upToDate = false;
								break;

							case REMOVE:
							{
								size_t index = (size_t)-1;
								if (m_vehicleSensors.GetSensorIndex(index, relativePosition))
								{
									size_t currentLastIndex = m_vehicleSensors.GetNumberOfSensors() - 1;
									if (m_currentSensorIndex == index && currentLastIndex == index)
									{
										--m_currentSensorIndex;
										if (currentLastIndex == 1)
											m_currentSensorAngle = DrawableVehicleBuilder::GetMinSensorAngle();
										else
											m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
									}
									m_vehicleSensors.RemoveSensor(index);
									m_upToDate = false;
								}
								break;
							}
						}

						break;
					}
				}
			}
		}
	}
}

void StateVehicleEditor::Update()
{
	auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsWriting())
	{
		if (!m_upToDate)
		{
			m_drawableVehicleBuilder.Clear();
			for (const auto& point : m_vehicleBody.m_points)
				m_drawableVehicleBuilder.AddVehicleBodyPoint(point);
			size_t numberOfSensors = m_vehicleSensors.GetNumberOfSensors();
			for (size_t i = 0; i < numberOfSensors; ++i)
				m_drawableVehicleBuilder.AddVehicleSensor(m_vehicleSensors.m_points[i], m_vehicleSensors.m_angleVector[i]);

			bool success = m_drawableVehicleBuilder.Save(filenameText->GetFilename());
			auto status = m_drawableVehicleBuilder.GetLastOperationStatus();
			filenameText->ShowStatusText();
			if (success)
				filenameText->SetSuccessStatusText(status.second);
			else
				filenameText->SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (filenameText->IsReading())
	{
		bool success = m_drawableVehicleBuilder.Load(filenameText->GetFilename());
		auto status = m_drawableVehicleBuilder.GetLastOperationStatus();
		filenameText->ShowStatusText();
		if (success)
		{
			filenameText->SetSuccessStatusText(status.second);
			m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
			m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
			m_currentSensorIndex = 0;
			m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
			m_upToDate = true;
		}
		else
			filenameText->SetErrorStatusText(status.second);
	}

	m_vehicleBody.Update();
	m_vehicleSensors.Update();

	for (const auto& text : m_texts)
		text->Update();
}

bool StateVehicleEditor::Load()
{
	// Create texts
	m_texts[BACK_TEXT] = new DrawableVariableText({ "Back" });
	m_texts[FRONT_TEXT] = new DrawableVariableText({ "Front" });
	m_texts[ACTIVE_MODE_TEXT] = new DrawableTripleText({ "Active mode:", "", "| [F1] [F2]" });
	m_texts[ACTIVE_SUBMODE_TEXT] = new DrawableTripleText({ "Active submode:", "", "| [1] [2]" });
	m_texts[TOTAL_NUMBER_OF_EDGES_TEXT] = new DrawableTripleText({ "Total number of edges:", "", "| [RMB]" });
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT] = new DrawableTripleText({ "Total number of sensors:", "", "| [RMB]" });
	m_texts[FILENAME_TEXT] = new DrawableFilenameText<true, true>;
	m_texts[CURRENT_SENSOR_TEXT] = new DrawableTripleText({ "Current sensor:", "", "| [Alt]" });
	m_texts[CURRENT_SENSOR_ANGLE_TEXT] = new DrawableTripleText({ "Current sensor angle:", "", "| [Z] [X]" });

	// Create observers
	m_textObservers[ACTIVE_MODE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_modeStrings[m_mode]; }, 0.2);
	m_textObservers[ACTIVE_SUBMODE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_submodeStrings[m_submode]; }, 0.2);
	m_textObservers[TOTAL_NUMBER_OF_EDGES_TEXT] = new FunctionTimerObserver<std::string>([&] { return std::to_string(m_vehicleBody.GetNumberOfPoints()); }, 0.1);
	m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT] = new FunctionTimerObserver<std::string>([&] { return std::to_string(m_vehicleSensors.GetNumberOfSensors()); }, 0.1);
	m_textObservers[CURRENT_SENSOR_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_vehicleSensors.GetNumberOfSensors() > 0 ? "S" + std::to_string(m_currentSensorIndex) : "None"; }, 0.1);
	m_textObservers[CURRENT_SENSOR_ANGLE_TEXT] = new FunctionTimerObserver<std::string>([&] { return std::to_string(size_t(m_currentSensorAngle)); }, 0.1);

	// Set text observers
	for (size_t i = ACTIVE_MODE_TEXT; i < TEXT_COUNT; ++i)
		((DrawableDoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);

	// Set text character size and rotation
	auto* backText = static_cast<DrawableVariableText*>(m_texts[BACK_TEXT]);
	auto* frontText = static_cast<DrawableVariableText*>(m_texts[FRONT_TEXT]);
	backText->SetCharacterSize(4);
	frontText->SetCharacterSize(4);
	backText->SetRotation(270.0f);
	frontText->SetRotation(90.0f);

	// Set text positions
	m_texts[BACK_TEXT]->SetPosition({ FontContext::Component(25), {2} });
	m_texts[FRONT_TEXT]->SetPosition({ FontContext::Component(18), {2, true} });
	m_texts[ACTIVE_MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {5}, {9} });
	m_texts[ACTIVE_SUBMODE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {5}, {9} });
	m_texts[TOTAL_NUMBER_OF_EDGES_TEXT]->SetPosition({ FontContext::Component(2), {0}, {5}, {9} });
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT]->SetPosition({ FontContext::Component(3), {0}, {5}, {9} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(4), {0}, {5}, {9}, {18} });
	m_texts[CURRENT_SENSOR_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {5}, {7} });
	m_texts[CURRENT_SENSOR_ANGLE_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {5}, {7} });

	CoreLogger::PrintSuccess("State \"Vehicle Editor\" dependencies loaded correctly");
	return true;
}

void StateVehicleEditor::Draw()
{
	m_vehicleBody.Draw();
	m_vehicleSensors.DrawBeams();
	m_vehicleSensors.DrawSensors();

	CoreWindow::GetRenderWindow().draw(m_xAxis.data(), m_xAxis.size(), sf::Lines);
	CoreWindow::GetRenderWindow().draw(m_yAxis.data(), m_xAxis.size(), sf::Lines);
	CoreWindow::GetRenderWindow().draw(m_allowedAreaShape);

	m_texts[BACK_TEXT]->Draw();
	m_texts[FRONT_TEXT]->Draw();
	m_texts[ACTIVE_MODE_TEXT]->Draw();
	m_texts[ACTIVE_SUBMODE_TEXT]->Draw();
	m_texts[TOTAL_NUMBER_OF_EDGES_TEXT]->Draw();
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT]->Draw();
	m_texts[FILENAME_TEXT]->Draw();
	
	if (m_mode == MODE_VEHICLE_SENSORS)
	{
		m_texts[CURRENT_SENSOR_TEXT]->Draw();
		m_texts[CURRENT_SENSOR_ANGLE_TEXT]->Draw();
	}
}