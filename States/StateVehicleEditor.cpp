#include "StateVehicleEditor.hpp"
#include "FunctionTimerObserver.hpp"
#include "CoreLogger.hpp"

StateVehicleEditor::StateVehicleEditor() :
	m_angleOffset(15.0) // 15 degrees
{
	m_submodeStrings[SUBMODE_VEHICLE_BODY] = "Vehicle body";
	m_submodeStrings[SUBMODE_VEHICLE_SENSORS] = "Vehicle sensors";
	m_submode = SUBMODE_VEHICLE_BODY;
	m_submodeState = INSERT;

	m_controlKeys[sf::Keyboard::Num1] = CHANGE_TO_INSERT_STATE;
	m_controlKeys[sf::Keyboard::Numpad1] = CHANGE_TO_INSERT_STATE;
	m_controlKeys[sf::Keyboard::Num2] = CHANGE_TO_REMOVE_STATE;
	m_controlKeys[sf::Keyboard::Numpad2] = CHANGE_TO_REMOVE_STATE;
	m_controlKeys[sf::Keyboard::Escape] = ESCAPE;
	m_controlKeys[sf::Keyboard::LAlt] = FIND_NEAREST_POINT;
	m_controlKeys[sf::Keyboard::RAlt] = FIND_NEAREST_POINT;
	m_controlKeys[sf::Keyboard::Z] = DEACREASE_SENSOR_ANGLE;
	m_controlKeys[sf::Keyboard::X] = INCREASE_SENSOR_ANGLE;

	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;


	if (m_drawableVehicleBuilder.CreateDummy())
	{
		m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
		m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
	}

	m_upToDate = false;
	m_textFunctions.reserve(16U);
}

StateVehicleEditor::~StateVehicleEditor()
{
}

void StateVehicleEditor::Reload()
{
	m_submode = SUBMODE_VEHICLE_BODY;
	m_submodeState = INSERT;

	// Reset pressed keys
	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset subjects of change
	if (m_drawableVehicleBuilder.CreateDummy())
	{
		m_vehicleBody = m_drawableVehicleBuilder.GetVehicleBody();
		m_vehicleSensors = m_drawableVehicleBuilder.GetVehicleSensors();
	}
	m_upToDate = false;

	// Reset texts
	m_totalNumberOfEdges.ResetObserverTimer();
	m_totalNumberOfSensors.ResetObserverTimer();
	m_filenameText.Reset();
	m_filenameText.ResetObserverTimer();
	m_currentSensorName.ResetObserverTimer();
	m_currentSensorAngle.ResetObserverTimer();

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
					case CHANGE_TO_INSERT_STATE:
						m_submodeState = INSERT;
						break;

					case CHANGE_TO_REMOVE_STATE:
						m_submodeState = REMOVE;
						break;

					case ESCAPE:
						break;

					case FIND_NEAREST_POINT:
						break;

					case DEACREASE_SENSOR_ANGLE:
						break;

					case INCREASE_SENSOR_ANGLE:
						break;
				}
			}
		}

		if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			auto iterator = m_controlKeys.find(eventKey);
			if (iterator != m_controlKeys.end())
				m_pressedKeys[iterator->second] = false;
		}
	}
}

void StateVehicleEditor::Update()
{
	if (m_filenameText.IsWriting())
	{
		if (!m_upToDate)
		{
			bool success = true;
			m_upToDate = success;
		}
	}
	else if (m_filenameText.IsReading())
	{
		
	}

	m_filenameText.Update();

	m_vehicleBody.Update();
	m_vehicleSensors.Update();
	m_backText.Update();
	m_frontText.Update();
	m_totalNumberOfEdges.Update();
	m_totalNumberOfSensors.Update();
	m_filenameText.Update();
	m_currentSensorName.Update();
	m_currentSensorAngle.Update();
}

bool StateVehicleEditor::Load()
{
	// Set consistent texts
	m_backText.SetText("Back");
	m_frontText.SetText("Front");
	m_totalNumberOfEdges.SetConsistentText("Total number of edges:");
	m_totalNumberOfSensors.SetConsistentText("Total number of sensors:");
	m_currentSensorName.SetConsistentText("Current sensor name:");
	m_currentSensorAngle.SetConsistentText("Current sensor angle:");

	// Set variable texts
	m_textFunctions.push_back([&] { return std::to_string(m_vehicleBody.GetNumberOfPoints()); });
	m_totalNumberOfEdges.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.1));

	// Set information texts
	m_totalNumberOfEdges.SetInformationText("| [1] [2] [RMB] [Ctrl] [Esc]");
	m_totalNumberOfSensors.SetInformationText("| [1] [2] [RMB]");
	m_currentSensorAngle.SetInformationText("| [Z] [X]");

	// Set text character size and rotation
	m_backText.SetCharacterSize(4);
	m_frontText.SetCharacterSize(4);
	m_backText.SetRotation(270.0f);
	m_frontText.SetRotation(90.0f);

	// Set text positions
	m_backText.SetPosition({ FontContext::Component(2), {25} });
	m_frontText.SetPosition({ FontContext::Component(2, true), {18} });
	m_totalNumberOfEdges.SetPosition({ FontContext::Component(0), {5}, {8}, {0} });
	m_totalNumberOfSensors.SetPosition({ FontContext::Component(0), {5}, {8}, {1} });
	m_filenameText.SetPosition({ FontContext::Component(0), {5}, {8}, {16}, {2} });
	m_currentSensorName.SetPosition({ FontContext::Component(0), {3}, {1, true} });
	m_currentSensorAngle.SetPosition({ FontContext::Component(0), {3}, {7}, {2, true} });

	CoreLogger::PrintSuccess("State \"Vehicle Editor\" dependencies loaded correctly");
	return true;
}

void StateVehicleEditor::Draw()
{
	m_vehicleBody.Draw();
	m_vehicleSensors.DrawBeams();
	m_vehicleSensors.DrawSensors();
	m_backText.Draw();
	m_frontText.Draw();
	m_totalNumberOfEdges.Draw();
	m_totalNumberOfSensors.Draw();
	m_filenameText.Draw();

	if (m_submode == SUBMODE_VEHICLE_SENSORS)
	{
		m_currentSensorName.Draw();
		m_currentSensorAngle.Draw();
	}
}