#include "StateVehicleEditor.hpp"
#include "FunctionEventObserver.hpp"
#include "ConsistentText.hpp"
#include "FilenameText.hpp"
#include "CoreLogger.hpp"
#include <functional>

StateVehicleEditor::StateVehicleEditor() : 
	m_axesPrecision(8.f)
{
	m_modeStrings[MODE_VEHICLE_BODY] = "Vehicle body";
	m_modeStrings[MODE_VEHICLE_SENSORS] = "Vehicle sensors";
	m_mode = MODE_VEHICLE_BODY;
	m_vehicleSensorsSubmodeStrings[VEHICLE_SENSORS_INSERT] = "Inserting";
	m_vehicleSensorsSubmodeStrings[VEHICLE_SENSORS_REMOVE] = "Removing";
	m_vehicleSensorsSubmode = VEHICLE_SENSORS_INSERT;

	// Initialize control keys
	m_controlKeys.insert(std::pair(sf::Keyboard::F1, CHANGE_TO_VEHICLE_BODY_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::F2, CHANGE_TO_VEHICLE_SENSORS_MODE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Num1, CHANGE_TO_INSERT_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Numpad1, CHANGE_TO_INSERT_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Num2, CHANGE_TO_REMOVE_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::Numpad2, CHANGE_TO_REMOVE_STATE));
	m_controlKeys.insert(std::pair(sf::Keyboard::BackSpace, REMOVE_LAST_VEHICLE_BODY_POINT));
	m_controlKeys.insert(std::pair(sf::Keyboard::RAlt, CHANGE_SENSOR));
	m_controlKeys.insert(std::pair(sf::Keyboard::LAlt, CHANGE_SENSOR));
	m_controlKeys.insert(std::pair(sf::Keyboard::Z, DEACREASE_SENSOR_ANGLE));
	m_controlKeys.insert(std::pair(sf::Keyboard::X, INCREASE_SENSOR_ANGLE));
	m_controlKeys.insert(std::pair(sf::Keyboard::C, DECREASE_SENSOR_MOTION_RANGE));
	m_controlKeys.insert(std::pair(sf::Keyboard::V, INCREASE_SENSOR_MOTION_RANGE));

	for (auto& i : m_pressedKeys)
		i = false;

	const auto maxVehicleSize = VehicleBuilder::GetMaxBodyBound();
	const auto windowSize = CoreWindow::GetWindowSize();
	const float allowedAreaX = (windowSize.x / 2.0f - maxVehicleSize.x / 2.0f);
	const float allowedAreaY = (windowSize.y / 2.0f - maxVehicleSize.y / 2.0f);
	m_allowedAreaShape.setFillColor(ColorContext::ClearBackground);
	m_allowedAreaShape.setOutlineColor(ColorContext::Grid);
	m_allowedAreaShape.setOutlineThickness(2);
	m_allowedAreaShape.setSize(maxVehicleSize);
	m_allowedAreaShape.setPosition(allowedAreaX, allowedAreaY);
	
	m_vehicleBuilder.CreateDummy();
	m_vehiclePrototype = m_vehicleBuilder.Get();

	m_currentSensorIndex = 0;
	m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
	m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
	m_upToDate = false;

	// Initialize grid
	const size_t numberOfGridAxes = size_t(m_axesPrecision);
	const size_t totalNumberOfGridAxes = numberOfGridAxes * 2;
	m_verticalOffset = maxVehicleSize.y / m_axesPrecision;
	m_horizontalOffset = maxVehicleSize.x / m_axesPrecision;
	m_axes.resize(totalNumberOfGridAxes + 4);
	for (size_t i = 0; i < numberOfGridAxes; ++i)
	{
		auto& a = m_axes[i];
		a[0].color = a[1].color = ColorContext::Grid;
		a[0].position = sf::Vector2f(allowedAreaX + m_horizontalOffset * i + (m_horizontalOffset / 2.f), allowedAreaY);
		a[1].position = sf::Vector2f(a[0].position.x, allowedAreaY + maxVehicleSize.y);

		auto& b = m_axes[i + numberOfGridAxes];
		b[0].color = b[1].color = ColorContext::Grid;
		b[0].position = sf::Vector2f(allowedAreaX, allowedAreaY + m_verticalOffset * i + (m_verticalOffset / 2.f));
		b[1].position = sf::Vector2f(allowedAreaX + maxVehicleSize.x, b[0].position.y);
	}

	// Add axes
	auto windowCenter = CoreWindow::GetWindowCenter();
	const auto offset = totalNumberOfGridAxes;
	m_axes[offset][0].color = m_axes[offset][1].color = ColorContext::Grid;
	m_axes[offset + 1][0].color = m_axes[offset + 1][1].color = ColorContext::Grid;
	m_axes[offset + 2][0].color = m_axes[offset + 2][1].color = ColorContext::Grid;
	m_axes[offset + 3][0].color = m_axes[offset + 3][1].color = ColorContext::Grid;
	m_axes[offset][0].position.x = m_axes[offset][1].position.x = m_axes[offset + 1][0].position.x = m_axes[offset + 1][1].position.x = windowCenter.x;
	m_axes[offset + 2][0].position.y = m_axes[offset + 2][1].position.y = m_axes[offset + 3][0].position.y = m_axes[offset + 3][1].position.y = windowCenter.y;
	m_axes[offset][0].position.y = m_axes[offset + 2][0].position.x = 0;
	m_axes[offset + 1][1].position.y = windowSize.y;
	m_axes[offset + 3][1].position.x = windowSize.x;
	m_axes[offset][1].position.y = allowedAreaY;
	m_axes[offset + 1][0].position.y = allowedAreaY + maxVehicleSize.y;
	m_axes[offset + 2][1].position.x = allowedAreaX;
	m_axes[offset + 3][0].position.x = allowedAreaX + maxVehicleSize.x;

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateVehicleEditor::~StateVehicleEditor()
{
	delete m_vehiclePrototype;
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateVehicleEditor::Reload()
{
	m_mode = MODE_VEHICLE_BODY;
	m_vehicleSensorsSubmode = VEHICLE_SENSORS_INSERT;

	// Reset pressed keys
	for (size_t i = 0; i < CONTROL_KEYS_COUNT; ++i)
		m_pressedKeys[i] = false;

	// Reset subjects of change
	m_vehicleBuilder.CreateDummy();
	delete m_vehiclePrototype;
	m_vehiclePrototype = m_vehicleBuilder.Get();

	m_currentSensorIndex = 0;
	m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
	m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
	m_upToDate = false;
	CalculateSupportiveShapes();

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}

	CoreWindow::Reset();
}

void StateVehicleEditor::Capture()
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
				m_pressedKeys[iterator->second] = true;
				switch (iterator->second)
				{
					case CHANGE_TO_VEHICLE_BODY_MODE:
					case CHANGE_TO_VEHICLE_SENSORS_MODE:
						m_mode = iterator->second == CHANGE_TO_VEHICLE_BODY_MODE ? MODE_VEHICLE_BODY : MODE_VEHICLE_SENSORS;
						m_vehicleSensorsSubmode = VEHICLE_SENSORS_INSERT;
						m_currentSensorIndex = 0;
						m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
						m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
						// Notify observers
						m_textObservers[MODE_TEXT]->Notify();
						m_textObservers[VEHICLE_SENSORS_SUBMODE_TEXT]->Notify();
						m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
						m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
						m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
						break;
					case CHANGE_TO_INSERT_STATE:
						if (m_mode == MODE_VEHICLE_SENSORS)
						{
							m_vehicleSensorsSubmode = VEHICLE_SENSORS_INSERT;
							m_textObservers[VEHICLE_SENSORS_SUBMODE_TEXT]->Notify();
						}
						break;
					case CHANGE_TO_REMOVE_STATE:
						if (m_mode == MODE_VEHICLE_SENSORS)
						{
							m_vehicleSensorsSubmode = VEHICLE_SENSORS_REMOVE;
							m_textObservers[VEHICLE_SENSORS_SUBMODE_TEXT]->Notify();
						}
						break;
					case REMOVE_LAST_VEHICLE_BODY_POINT:
						if (m_vehiclePrototype->RemoveLastBodyPoint())
						{
							auto sensorPoints = m_vehiclePrototype->GetSensorPoints();
							for (const auto& point : sensorPoints)
							{
								if (!MathContext::IsPointInsidePolygon(m_vehiclePrototype->GetBodyPoints(), point))
									RemoveSensor(point);
							}

							CalculateSupportiveShapes();
							m_textObservers[TOTAL_NUMBER_OF_BODY_POINTS_TEXT]->Notify();
							m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT]->Notify();
							m_textObservers[VEHICLE_BODY_MASS]->Notify();
							m_upToDate = false;
						}
						break;
					case CHANGE_SENSOR:
						if (m_mode != MODE_VEHICLE_SENSORS)
							break;
						++m_currentSensorIndex;
						if (m_currentSensorIndex >= m_vehiclePrototype->GetNumberOfSensors())
							m_currentSensorIndex = 0;
						m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
						m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
						m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
						m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
						m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
						m_upToDate = false;
						break;
					case DEACREASE_SENSOR_ANGLE:
						if (m_mode != MODE_VEHICLE_SENSORS)
							break;
						if (m_vehiclePrototype->GetNumberOfSensors() > 0)
						{
							m_currentSensorAngle -= VehicleBuilder::GetDefaultSensorAngleOffset();
							if (m_currentSensorAngle < VehicleBuilder::GetMinSensorAngle())
								m_currentSensorAngle = VehicleBuilder::GetMaxSensorAngle() - VehicleBuilder::GetDefaultSensorAngleOffset();
							m_vehiclePrototype->SetSensorBeamAngle(m_currentSensorIndex, m_currentSensorAngle);
							m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
							m_upToDate = false;
						}
						break;
					case INCREASE_SENSOR_ANGLE:
						if (m_mode != MODE_VEHICLE_SENSORS)
							break;
						if (m_vehiclePrototype->GetNumberOfSensors() > 0)
						{
							m_currentSensorAngle += VehicleBuilder::GetDefaultSensorAngleOffset();
							if (m_currentSensorAngle > VehicleBuilder::GetMaxSensorAngle())
								m_currentSensorAngle = VehicleBuilder::GetMinSensorAngle() + VehicleBuilder::GetDefaultSensorAngleOffset();
							else if (m_currentSensorAngle == VehicleBuilder::GetMaxSensorAngle())
								m_currentSensorAngle = VehicleBuilder::GetMinSensorAngle();
							m_vehiclePrototype->SetSensorBeamAngle(m_currentSensorIndex, m_currentSensorAngle);
							m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
							m_upToDate = false;
						}
						break;
					case DECREASE_SENSOR_MOTION_RANGE:
						if (m_mode != MODE_VEHICLE_SENSORS)
							break;
						if (m_vehiclePrototype->GetNumberOfSensors() > 0)
						{
							m_currentSensorMotionRange -= VehicleBuilder::GetDefaultSensorMotionRange();
							if (m_currentSensorMotionRange < VehicleBuilder::GetMinSensorMotionRange())
								m_currentSensorMotionRange = VehicleBuilder::GetMinSensorMotionRange();
							m_vehiclePrototype->SetSensorMotionRange(m_currentSensorIndex, m_currentSensorMotionRange);
							m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
							m_upToDate = false;
						}
						break;
					case INCREASE_SENSOR_MOTION_RANGE:
						if (m_mode != MODE_VEHICLE_SENSORS)
							break;
						if (m_vehiclePrototype->GetNumberOfSensors() > 0)
						{
							m_currentSensorMotionRange += VehicleBuilder::GetDefaultSensorMotionRange();
							if (m_currentSensorMotionRange > VehicleBuilder::GetMaxSensorMotionRange())
								m_currentSensorMotionRange = VehicleBuilder::GetMaxSensorMotionRange();
							m_vehiclePrototype->SetSensorMotionRange(m_currentSensorIndex, m_currentSensorMotionRange);
							m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
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
			if (MathContext::IsPointInsideRectangle(m_allowedAreaShape.getSize(), m_allowedAreaShape.getPosition(), relativePosition))
			{
				switch (m_mode)
				{
					case MODE_VEHICLE_BODY:
					{
						if (m_vehiclePrototype->GetNumberOfBodyPoints() >= VehicleBuilder::GetMaxNumberOfBodyPoints())
							break;
						CalculateGridPoint(relativePosition);
						relativePosition -= CoreWindow::GetWindowCenter();
						if (m_vehiclePrototype->AddBodyPoint(relativePosition))
						{
							CalculateSupportiveShapes();
							m_textObservers[TOTAL_NUMBER_OF_BODY_POINTS_TEXT]->Notify();
							m_textObservers[VEHICLE_BODY_MASS]->Notify();
							m_upToDate = false;
						}
						
						break;
					}
					case MODE_VEHICLE_SENSORS:
					{
						relativePosition -= CoreWindow::GetWindowCenter();
						switch (m_vehicleSensorsSubmode)
						{
							case VEHICLE_SENSORS_INSERT:
								InsertSensor(relativePosition);
								break;
							case VEHICLE_SENSORS_REMOVE:
								RemoveSensor(relativePosition);
								break;
						}

						break;
					}
				}
			}
		}
		else if (CoreWindow::GetEvent().type == sf::Event::MouseMoved)
		{
			switch (m_mode)
			{
				case MODE_VEHICLE_BODY:
					CalculateSupportiveShapes();
					break;
				case MODE_VEHICLE_SENSORS:
				default:
					break;
			}
		}
	}
	else
		m_upToDate = false;
}

void StateVehicleEditor::Update()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsWriting())
	{
		if (!m_upToDate)
		{
			m_vehicleBuilder.Clear();
			for (const auto& point : m_vehiclePrototype->GetBodyPoints())
				m_vehicleBuilder.AddBodyPoint(point);
			size_t numberOfSensors = m_vehiclePrototype->GetNumberOfSensors();
			for (size_t i = 0; i < numberOfSensors; ++i)
				m_vehicleBuilder.AddSensor(m_vehiclePrototype->GetSensorPoint(i),
										   m_vehiclePrototype->GetSensorBeamAngle(i),
										   m_vehiclePrototype->GetSensorMotionRange(i));

			bool success = m_vehicleBuilder.Save(filenameText->GetFilename());
			auto status = m_vehicleBuilder.GetLastOperationStatus();
			if (success)
				filenameText->SetSuccessStatusText(status.second);
			else
				filenameText->SetErrorStatusText(status.second);
			m_upToDate = success;
		}
	}
	else if (filenameText->IsReading())
	{
		bool success = m_vehicleBuilder.Load(filenameText->GetFilename());
		auto status = m_vehicleBuilder.GetLastOperationStatus();
		if (success)
		{
			filenameText->SetSuccessStatusText(status.second);
			delete m_vehiclePrototype;
			m_vehiclePrototype = m_vehicleBuilder.Get();
			m_currentSensorIndex = 0;
			m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
			m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
			m_textObservers[TOTAL_NUMBER_OF_BODY_POINTS_TEXT]->Notify();
			m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT]->Notify();
			m_textObservers[VEHICLE_BODY_MASS]->Notify();
			m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
			m_upToDate = true;
		}
		else
			filenameText->SetErrorStatusText(status.second);
	}

	m_vehiclePrototype->Update();

	for (const auto& text : m_texts)
		text->Update();
}

bool StateVehicleEditor::Load()
{
	// Create texts
	m_texts[BACK_TEXT] = new ConsistentText({ "Back" });
	m_texts[FRONT_TEXT] = new ConsistentText({ "Front" });
	m_texts[MODE_TEXT] = new TripleText({ "Mode:", "", "| [F1] [F2]" });
	m_texts[TOTAL_NUMBER_OF_BODY_POINTS_TEXT] = new TripleText({ "Total number of body points:", "", "| [RMB] [Backspace]" });
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT] = new TripleText({ "Total number of sensors:", "", "| [RMB]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, true>("vehicle.bin");
	m_texts[VEHICLE_BODY_MASS] = new DoubleText({ "Vehicle body mass:", "" });
	m_texts[VEHICLE_SENSORS_SUBMODE_TEXT] = new TripleText({ "Submode:", "", "| [1] [2]" });
	m_texts[CURRENT_SENSOR_TEXT] = new TripleText({ "Current sensor:", "", "| [Alt]" });
	m_texts[CURRENT_SENSOR_ANGLE_TEXT] = new TripleText({ "Current sensor angle:", "", "| [Z] [X]" });
	m_texts[CURRENT_SENSOR_MOTION_RANGE_TEXT] = new TripleText({ "Current sensor motion range:", "", "| [C] [V]" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[TOTAL_NUMBER_OF_BODY_POINTS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_vehiclePrototype->GetNumberOfBodyPoints(); });
	m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT] = new FunctionEventObserver<size_t>([&] { return m_vehiclePrototype->GetNumberOfSensors(); });
	m_textObservers[VEHICLE_BODY_MASS] = new FunctionEventObserver<std::string>([&] { return m_vehiclePrototype->GetNumberOfBodyPoints() < VehicleBuilder::GetMinNumberOfBodyPoints() ? "Unknown" : std::to_string(VehicleBuilder::CalculateMass(m_vehiclePrototype->GetBodyPoints())) + " kg"; });
	m_textObservers[VEHICLE_SENSORS_SUBMODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehicleSensorsSubmodeStrings[m_vehicleSensorsSubmode]; });
	m_textObservers[CURRENT_SENSOR_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehiclePrototype->GetNumberOfSensors() > 0 ? "Sensor " + std::to_string(m_currentSensorIndex) : "None"; });
	m_textObservers[CURRENT_SENSOR_ANGLE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehiclePrototype->GetNumberOfSensors() > 0 ? std::to_string(size_t(m_currentSensorAngle)) : "Unknown"; });
	m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_vehiclePrototype->GetNumberOfSensors() > 0 ? std::to_string(m_currentSensorMotionRange) : "Unknown"; });

	// Set text observers
	for (size_t i = MODE_TEXT; i < TEXT_COUNT; ++i)
		((DoubleText*)m_texts[i])->SetObserver(m_textObservers[i]);

	// Set text character size and rotation
	auto* backText = static_cast<ConsistentText*>(m_texts[BACK_TEXT]);
	auto* frontText = static_cast<ConsistentText*>(m_texts[FRONT_TEXT]);
	backText->SetCharacterSize(4);
	frontText->SetCharacterSize(4);
	backText->SetRotation(270.0f);
	frontText->SetRotation(90.0f);

	// Set text positions
	m_texts[BACK_TEXT]->SetPosition({ FontContext::Component(25), {2} });
	m_texts[FRONT_TEXT]->SetPosition({ FontContext::Component(18), {2, true} });
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {6}, {10} });
	m_texts[TOTAL_NUMBER_OF_BODY_POINTS_TEXT]->SetPosition({ FontContext::Component(1), {0}, {6}, {10} });
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT]->SetPosition({ FontContext::Component(2), {0}, {6}, {10} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(3), {0}, {6}, {10}, {19} });
	m_texts[VEHICLE_BODY_MASS]->SetPosition({ FontContext::Component(1, true), {0}, {4} });
	m_texts[VEHICLE_SENSORS_SUBMODE_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {6}, {8} });
	m_texts[CURRENT_SENSOR_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {6}, {8} });
	m_texts[CURRENT_SENSOR_ANGLE_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {6}, {8} });
	m_texts[CURRENT_SENSOR_MOTION_RANGE_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {6}, {8} });

	CoreLogger::PrintSuccess("StateVehicleEditor dependencies loaded correctly");
	return true;
}

void StateVehicleEditor::Draw()
{
	m_vehiclePrototype->DrawBody();
	m_vehiclePrototype->DrawBeams();
	m_vehiclePrototype->DrawSensors();
	if (m_mode == MODE_VEHICLE_SENSORS)
		m_vehiclePrototype->DrawMarkedSensor(m_currentSensorIndex);
	else
	{
		CoreWindow::Draw(m_lineShape.data(), m_lineShape.size(), sf::Lines);
		CoreWindow::Draw(m_triangleShape.data(), m_triangleShape.size(), sf::Triangles);
	}

	for (auto & axis : m_axes)
		CoreWindow::Draw(axis.data(), axis.size(), sf::Lines);

	CoreWindow::Draw(m_allowedAreaShape);

	m_texts[BACK_TEXT]->Draw();
	m_texts[FRONT_TEXT]->Draw();
	m_texts[MODE_TEXT]->Draw();
	m_texts[TOTAL_NUMBER_OF_BODY_POINTS_TEXT]->Draw();
	m_texts[TOTAL_NUMBER_OF_SENSORS_TEXT]->Draw();
	m_texts[FILENAME_TEXT]->Draw();
	
	if (m_mode == MODE_VEHICLE_BODY)
	{
		m_texts[VEHICLE_BODY_MASS]->Draw();
	}
	else
	{
		m_texts[VEHICLE_SENSORS_SUBMODE_TEXT]->Draw();
		m_texts[CURRENT_SENSOR_TEXT]->Draw();
		m_texts[CURRENT_SENSOR_ANGLE_TEXT]->Draw();
		m_texts[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Draw();
	}
}

void StateVehicleEditor::InsertSensor(const sf::Vector2f& point)
{
	if (m_vehiclePrototype->GetNumberOfSensors() < VehicleBuilder::GetMaxNumberOfSensors())
	{
		if (MathContext::IsPointInsidePolygon(m_vehiclePrototype->GetBodyPoints(), point))
		{
			m_vehiclePrototype->AddSensor(point, VehicleBuilder::GetMinSensorAngle(), VehicleBuilder::GetDefaultSensorMotionRange());
			m_currentSensorIndex = m_vehiclePrototype->GetNumberOfSensors() - 1;
			m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
			m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
			m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
			m_upToDate = false;
		}
	}
}

void StateVehicleEditor::RemoveSensor(const sf::Vector2f& point)
{
	size_t index = (size_t)-1;
	if (m_vehiclePrototype->GetSensorIndex(index, point))
	{
		if (m_vehiclePrototype->GetNumberOfSensors() == 1)
		{
			// If it was last sensor index and it is first index also
			m_currentSensorAngle = VehicleBuilder::GetMinSensorAngle();
			m_currentSensorMotionRange = VehicleBuilder::GetDefaultSensorMotionRange();
			m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
			m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
		}
		else
		{
			// There are at least two sensors
			size_t currentLastIndex = m_vehiclePrototype->GetNumberOfSensors() - 1;
			if (m_currentSensorIndex == index && m_currentSensorIndex == currentLastIndex)
			{
				// If it was last sensor index but not first index then we have decrement it
				--m_currentSensorIndex;
				m_currentSensorAngle = m_vehiclePrototype->GetSensorBeamAngle(m_currentSensorIndex);
				m_currentSensorMotionRange = m_vehiclePrototype->GetSensorMotionRange(m_currentSensorIndex);
				m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
				m_textObservers[CURRENT_SENSOR_ANGLE_TEXT]->Notify();
				m_textObservers[CURRENT_SENSOR_MOTION_RANGE_TEXT]->Notify();
			}
			else if (m_currentSensorIndex > index)
			{
				// If current pointed index is greater then to be deleted one
				--m_currentSensorIndex;
				m_textObservers[CURRENT_SENSOR_TEXT]->Notify();
			}
		}

		m_vehiclePrototype->RemoveSensor(index);
		m_textObservers[TOTAL_NUMBER_OF_SENSORS_TEXT]->Notify();
		m_upToDate = false;
	}
}

void StateVehicleEditor::CalculateGridPoint(sf::Vector2f& point)
{
	const auto halfHorizontalOffset = (m_horizontalOffset / 2.f);
	const auto halfVerticalOffset = (m_verticalOffset / 2.f);
	const auto allowedAreaSize = m_allowedAreaShape.getSize();
	const auto allowedAreaPosition = m_allowedAreaShape.getPosition();
	const size_t x = size_t((point.x - (allowedAreaPosition.x - halfHorizontalOffset)) / m_horizontalOffset);
	const size_t y = size_t((point.y - (allowedAreaPosition.y - halfVerticalOffset)) / m_verticalOffset);
	point.x = float(x) * m_horizontalOffset + allowedAreaPosition.x;
	point.y = float(y) * m_verticalOffset + allowedAreaPosition.y;
}

void StateVehicleEditor::CalculateSupportiveShapes()
{
	// Reset positions
	m_lineShape[0].position = m_lineShape[1].position = sf::Vector2f(0.f, 0.f);
	m_triangleShape[0].position = m_triangleShape[1].position = m_triangleShape[2].position = sf::Vector2f(0.f, 0.f);

	size_t numberOfBodyPoints = m_vehiclePrototype->GetNumberOfBodyPoints();
	auto relativePosition = CoreWindow::GetMousePosition();
	if (MathContext::IsPointInsideRectangle(m_allowedAreaShape.getSize(), m_allowedAreaShape.getPosition(), relativePosition))
	{
		CalculateGridPoint(relativePosition);
		const auto firstBodyPoint = m_vehiclePrototype->GetBodyPoint(0) + CoreWindow::GetWindowCenter();
		if (numberOfBodyPoints == 1)
		{
			m_lineShape[0].position = firstBodyPoint;
			m_lineShape[1].position = relativePosition;
		}
		else if (numberOfBodyPoints == 2)
		{
			const float epsilon = 1.f;
			const auto lastBodyPoint = m_vehiclePrototype->GetBodyPoint(numberOfBodyPoints - 1) + CoreWindow::GetWindowCenter();
			const auto distance = MathContext::Distance(firstBodyPoint, lastBodyPoint);
			const auto approximate = MathContext::Distance(firstBodyPoint, relativePosition) + MathContext::Distance(lastBodyPoint, relativePosition);
			if (std::fabs(distance - approximate) < epsilon)
			{
				m_lineShape[0].position = firstBodyPoint;
				m_lineShape[1].position = lastBodyPoint;
			}

			m_triangleShape[0].position = firstBodyPoint;
			m_triangleShape[1].position = m_vehiclePrototype->GetBodyPoint(numberOfBodyPoints - 1) + CoreWindow::GetWindowCenter();
			m_triangleShape[2].position = relativePosition;
		}
		else if (numberOfBodyPoints > 1)
		{
			m_triangleShape[0].position = firstBodyPoint;
			m_triangleShape[1].position = m_vehiclePrototype->GetBodyPoint(numberOfBodyPoints - 1) + CoreWindow::GetWindowCenter();
			m_triangleShape[2].position = relativePosition;
		}
	}
}
