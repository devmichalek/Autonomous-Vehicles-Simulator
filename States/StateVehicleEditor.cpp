#include "StateVehicleEditor.hpp"
#include "FunctionTimerObserver.hpp"
#include "CoreLogger.hpp"

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
	
	if (m_drawableVehicleBuilder.CreateDummy())
	{
		m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
		m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
	}
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

	m_textFunctions.reserve(16U);
}

StateVehicleEditor::~StateVehicleEditor()
{
}

void StateVehicleEditor::Reload()
{
	m_mode = MODE_VEHICLE_BODY;
	m_submode = INSERT;

	// Reset pressed keys
	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset subjects of change
	if (m_drawableVehicleBuilder.CreateDummy())
	{
		m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
		m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
	}
	m_currentSensorIndex = m_vehicleSensors.GetNumberOfSensors() - 1;
	m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
	m_upToDate = false;

	// Reset texts
	m_totalNumberOfEdgesText.Reset();
	m_totalNumberOfSensorsText.Reset();
	m_filenameText.Reset();
	m_currentSensorText.Reset();
	m_currentSensorAngleText.Reset();

	// Reset view
	auto& view = CoreWindow::GetView();
	auto viewOffset = CoreWindow::GetViewOffset();
	view.move(-viewOffset);
	CoreWindow::GetRenderWindow().setView(view);
}

void StateVehicleEditor::Capture()
{
	m_filenameText.Capture();
	if (!m_filenameText.IsRenaming())
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
	if (m_filenameText.IsWriting())
	{
		if (!m_upToDate)
		{
			m_drawableVehicleBuilder.Clear();
			for (const auto& point : m_vehicleBody.m_points)
				m_drawableVehicleBuilder.AddVehicleBodyPoint(point);
			size_t numberOfSensors = m_vehicleSensors.GetNumberOfSensors();
			for (size_t i = 0; i < numberOfSensors; ++i)
				m_drawableVehicleBuilder.AddVehicleSensor(m_vehicleSensors.m_points[i], m_vehicleSensors.m_angleVector[i]);

			bool success = m_drawableVehicleBuilder.Save(m_filenameText.GetFilename());
			auto status = m_drawableVehicleBuilder.GetLastOperationStatus();
			m_filenameText.ShowStatusText();
			if (success)
				m_filenameText.SetSuccessStatusText(status.second);
			else
				m_filenameText.SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (m_filenameText.IsReading())
	{
		bool success = m_drawableVehicleBuilder.Load(m_filenameText.GetFilename());
		auto status = m_drawableVehicleBuilder.GetLastOperationStatus();
		m_filenameText.ShowStatusText();
		if (success)
		{
			m_filenameText.SetSuccessStatusText(status.second);
			m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
			m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
			m_currentSensorIndex = 0;
			m_currentSensorAngle = m_vehicleSensors.GetSensorAngle(m_currentSensorIndex);
			m_upToDate = true;
		}
		else
			m_filenameText.SetErrorStatusText(status.second);
	}

	m_vehicleBody.Update();
	m_vehicleSensors.Update();
	m_backText.Update();
	m_frontText.Update();
	m_activeModeText.Update();
	m_activeSubmodeText.Update();
	m_totalNumberOfEdgesText.Update();
	m_totalNumberOfSensorsText.Update();
	m_filenameText.Update();
	m_currentSensorText.Update();
	m_currentSensorAngleText.Update();
}

bool StateVehicleEditor::Load()
{
	// Set texts strings
	m_backText.SetStrings({ "Back" });
	m_frontText.SetStrings({ "Front" });
	m_activeModeText.SetStrings({ "Active mode:", "", "| [F1] [F2]" });
	m_activeSubmodeText.SetStrings({ "Active submode:", "", "| [1] [2]" });
	m_totalNumberOfEdgesText.SetStrings({ "Total number of edges:", "", "| [RMB]" });
	m_totalNumberOfSensorsText.SetStrings({ "Total number of sensors:", "", "| [RMB]" });
	m_currentSensorText.SetStrings({ "Current sensor:", "", "| [Alt]" });
	m_currentSensorAngleText.SetStrings({ "Current sensor angle:", "", "| [Z] [X]" });

	// Set variable texts
	m_textFunctions.push_back([&] { return m_modeStrings[m_mode]; });
	m_activeModeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.2));
	m_textFunctions.push_back([&] { return m_submodeStrings[m_submode]; });
	m_activeSubmodeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.2));
	m_textFunctions.push_back([&] { return std::to_string(m_vehicleBody.GetNumberOfPoints()); });
	m_totalNumberOfEdgesText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));
	m_textFunctions.push_back([&] { return std::to_string(m_vehicleSensors.GetNumberOfSensors()); });
	m_totalNumberOfSensorsText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));
	m_textFunctions.push_back([&] { return m_vehicleSensors.GetNumberOfSensors() > 0 ? "S" + std::to_string(m_currentSensorIndex) : "None"; });
	m_currentSensorText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));
	m_textFunctions.push_back([&] { return std::to_string(size_t(m_currentSensorAngle)); });
	m_currentSensorAngleText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));

	// Set text character size and rotation
	m_backText.SetCharacterSize(4);
	m_frontText.SetCharacterSize(4);
	m_backText.SetRotation(270.0f);
	m_frontText.SetRotation(90.0f);

	// Set text positions
	m_backText.SetPosition({ FontContext::Component(25), {2} });
	m_frontText.SetPosition({ FontContext::Component(18), {2, true} });
	m_activeModeText.SetPosition({ FontContext::Component(0), {0}, {5}, {9} });
	m_activeSubmodeText.SetPosition({ FontContext::Component(1), {0}, {5}, {9} });
	m_totalNumberOfEdgesText.SetPosition({ FontContext::Component(2), {0}, {5}, {9} });
	m_totalNumberOfSensorsText.SetPosition({ FontContext::Component(3), {0}, {5}, {9} });
	m_filenameText.SetPosition({ FontContext::Component(4), {0}, {5}, {9}, {18} });
	m_currentSensorText.SetPosition({ FontContext::Component(1, true), {0}, {5}, {7} });
	m_currentSensorAngleText.SetPosition({ FontContext::Component(2, true), {0}, {5}, {7} });

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

	m_backText.Draw();
	m_frontText.Draw();
	m_activeModeText.Draw();
	m_activeSubmodeText.Draw();
	m_totalNumberOfEdgesText.Draw();
	m_totalNumberOfSensorsText.Draw();
	m_filenameText.Draw();

	if (m_mode == MODE_VEHICLE_SENSORS)
	{
		m_currentSensorText.Draw();
		m_currentSensorAngleText.Draw();
	}
}