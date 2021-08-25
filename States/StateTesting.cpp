#pragma once
#include "StateTesting.hpp"
#include "CoreLogger.hpp"
#include "FunctionTimerObserver.hpp"

StateTesting::StateTesting()
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_mode = STOPPED_MODE;
	m_filenameTypeStrings[MAP_FILENAME_TYPE] = "Map";
	m_filenameTypeStrings[ANN_FILENAME_TYPE] = "ANN";
	m_filenameTypeStrings[VEHICLE_FILENAME_TYPE] = "Vehicle";
	m_filenameType = MAP_FILENAME_TYPE;
	m_modeKey = std::make_pair(sf::Keyboard::M, false);
	m_filenameTypeKey = std::make_pair(sf::Keyboard::F, false);
	m_drawableVehicleBuilder.CreateDummy();
	m_drawableMap = nullptr;
	m_userVehicle = nullptr;
	m_drawableVehicleFactory.resize(2U);
	m_drawableCheckpointMap = nullptr;
	m_textFunctions.reserve(16U);
}

StateTesting::~StateTesting()
{
	delete m_drawableMap;
	delete m_userVehicle;
	delete m_drawableCheckpointMap;
}

void StateTesting::Reload()
{
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;
	m_modeKey.second = false;
	m_filenameTypeKey.second = false;
	m_drawableMapBuilder.Clear();
	delete m_drawableMap;
	m_drawableMap = nullptr;
	delete m_userVehicle;
	m_userVehicle = nullptr;
	delete m_drawableCheckpointMap;
	m_drawableCheckpointMap = nullptr;
}

void StateTesting::Capture()
{
	if (m_filenameText.IsRenaming())
		m_filenameText.Capture();
	else
	{
		if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			switch (m_mode)
			{
				case STOPPED_MODE:
				{
					if (m_filenameTypeKey.first == eventKey)
						m_filenameTypeKey.second = true;
					else
						m_filenameText.Capture();

					break;
				}
				case RUNNING_MODE:
				{
					break;
				}
				default:
					break;
			}

			if (m_modeKey.first == eventKey)
				m_modeKey.second = true;
		}
		else if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
		{
			auto eventKey = CoreWindow::GetEvent().key.code;
			switch (m_mode)
			{
				case STOPPED_MODE:
				{
					if (m_filenameTypeKey.first == eventKey)
						m_filenameTypeKey.second = false;
					else
						m_filenameText.Capture();
					break;
				}
				case RUNNING_MODE:
				{
					break;
				}
				default:
					break;
			}

			if (m_modeKey.first == eventKey)
				m_modeKey.second = false;
		}
	}
}

void StateTesting::Update()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			if (m_filenameTypeKey.second)
			{
				++m_filenameType;
				if (m_filenameType >= FILENAME_TYPES_COUNT)
					m_filenameType = MAP_FILENAME_TYPE;
				m_filenameTypeKey.second = false;
			}

			m_filenameTypeText.Update();
			m_filenameText.Update();

			if (!m_filenameText.IsRenaming())
			{
				if (m_filenameText.IsReading())
				{
					switch (m_filenameType)
					{
						case MAP_FILENAME_TYPE:
						{
							bool success = m_drawableMapBuilder.Load(m_filenameText.GetFilename());
							auto status = m_drawableMapBuilder.GetLastOperationStatus();
							m_filenameText.ShowStatusText();
							if (success)
							{
								m_filenameText.SetSuccessStatusText(status.second);
								delete m_drawableMap;
								m_drawableMap = m_drawableMapBuilder.GetDrawableMap();
								for (auto& vehicle : m_drawableVehicleFactory)
								{
									delete vehicle;
									vehicle = m_drawableVehicleBuilder.Get();
									auto details = m_drawableMapBuilder.GetVehicle();
									vehicle->SetCenter(details.first);
									vehicle->SetAngle(details.second);
									vehicle->Update();
								}

								m_userVehicle = m_drawableVehicleFactory[0];
								delete m_drawableCheckpointMap;
								m_drawableCheckpointMap = m_drawableMapBuilder.GetDrawableCheckpointMap();
								m_drawableCheckpointMap->Init(m_drawableVehicleFactory.size(), 0.0);

								for (auto& vehicle : m_drawableVehicleFactory)
									vehicle->Update();

								// Update view
								auto& view = CoreWindow::GetView();
								view.setCenter(m_userVehicle->GetCenter());
								CoreWindow::GetRenderWindow().setView(view);
							}
							else
								m_filenameText.SetErrorStatusText(status.second);
							break;
						}

						case ANN_FILENAME_TYPE:

							break;

						case VEHICLE_FILENAME_TYPE:

							break;

						default:
							break;
					}
				}
			}

			break;
		}
		case RUNNING_MODE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				m_drawableVehicleFactory.front()->SetActive();
				m_drawableCheckpointMap->Iterate(m_drawableVehicleFactory);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				m_userVehicle->Rotate(0.0);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				m_userVehicle->Rotate(1.0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				m_userVehicle->Accelerate(1.0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				m_userVehicle->Brake(1.0);
			}

			if (m_userVehicle)
			{
				// Update user vehicle even if it is destroyed
				m_userVehicle->Update();

				// Update computer's vehicles only if they are not destroyed
				for (size_t i = 1; i < m_drawableVehicleFactory.size(); ++i)
				{
					if (m_drawableVehicleFactory[i]->IsActive())
						m_drawableVehicleFactory[i]->Update();
				}

				// Update view
				auto& view = CoreWindow::GetView();
				view.setCenter(m_userVehicle->GetCenter());
				CoreWindow::GetRenderWindow().setView(view);
			}
			
			if (m_drawableMap)
				m_drawableMap->Intersect(m_drawableVehicleFactory);

			break;
		}
		default:
			break;
	}

	if (m_modeKey.second)
	{
		++m_mode;
		if (m_mode >= MODES_COUNT)
			m_mode = STOPPED_MODE;
		m_modeKey.second = false;
	}

	m_modeText.Update();
	m_fitnessText.Update();
}

bool StateTesting::Load()
{
	// Set texts strings
	m_modeText.SetStrings({ "Mode:", "", "| [M]" });
	m_fitnessText.SetStrings({ "Highest fitness:", "", "| [Enter]" });
	m_filenameTypeText.SetStrings({ "Filename type:", "", "| [F]" });

	// Set variable texts
	m_textFunctions.push_back([&] { return m_modeStrings[m_mode]; });
	m_modeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return m_drawableCheckpointMap ? std::to_string(m_drawableCheckpointMap->GetHighestFitness()) : "0"; });
	m_fitnessText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return m_filenameTypeStrings[m_filenameType]; });
	m_filenameTypeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.2));

	// Set texts positions
	m_modeText.SetPosition({ FontContext::Component(0), {0}, {3}, {7} });
	m_fitnessText.SetPosition({ FontContext::Component(1, true), {0}, {4}, {7} });
	m_filenameTypeText.SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_filenameText.SetPosition({ FontContext::Component(2), {0}, {3}, {7}, {13} });

	CoreLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::Draw()
{
	for (auto& vehicle : m_drawableVehicleFactory)
	{
		if (!vehicle || !vehicle->IsActive())
			continue;
		vehicle->DrawBody();
		vehicle->DrawBeams();
	}

	if (m_drawableMap)
	{
		m_drawableMap->Draw();
		m_drawableCheckpointMap->Draw();
	}

	switch (m_mode)
	{
		case STOPPED_MODE:
			m_filenameTypeText.Draw();
			m_filenameText.Draw();
			break;
		case RUNNING_MODE:
			break;
		default:
			break;
	}
	
	m_modeText.Draw();
	m_fitnessText.Draw();
}
