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
	m_filenameTypeStrings[CAR_FILENAME_TYPE] = "Car";
	m_filenameType = MAP_FILENAME_TYPE;
	m_modeKey = std::make_pair(sf::Keyboard::M, false);
	m_filenameTypeKey = std::make_pair(sf::Keyboard::F, false);
	m_edgeManager = nullptr;
	m_userCar = nullptr;
	m_carFactory.resize(2U);
	m_checkpointMap = nullptr;
	m_textFunctions.reserve(16U);
}

StateTesting::~StateTesting()
{
	delete m_edgeManager;
	delete m_userCar;
	delete m_checkpointMap;
}

void StateTesting::Reload()
{
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;
	m_modeKey.second = false;
	m_filenameTypeKey.second = false;
	m_drawableBuilder.Clear();
	delete m_edgeManager;
	m_edgeManager = nullptr;
	delete m_userCar;
	m_userCar = nullptr;
	delete m_checkpointMap;
	m_checkpointMap = nullptr;
}

void StateTesting::Capture()
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
							bool success = m_drawableBuilder.Load(m_filenameText.GetFilename());
							auto status = m_drawableBuilder.GetLastOperationStatus();
							m_filenameText.ShowStatusText();
							if (success)
							{
								m_filenameText.SetSuccessStatusText(status.second);
								delete m_edgeManager;
								m_edgeManager = m_drawableBuilder.GetDrawableManager();
								for (auto& car : m_carFactory)
								{
									delete car.first;
									car = std::pair(m_drawableBuilder.GetDrawableCar(), true);
									car.first->init(CAR_DEFAULT_NUMBER_OF_SENSORS);
									car.first->update();
								}

								m_userCar = m_carFactory[0].first;
								delete m_checkpointMap;
								m_checkpointMap = m_drawableBuilder.GetDrawableCheckpointMap();
								m_checkpointMap->restart(m_carFactory.size(), 0.0);

								for (auto& car : m_carFactory)
									car.first->update();

								// Update view
								auto& view = CoreWindow::GetView();
								view.setCenter(m_userCar->getCenter());
								CoreWindow::GetRenderWindow().setView(view);
							}
							else
								m_filenameText.SetErrorStatusText(status.second);
							break;
						}

						case ANN_FILENAME_TYPE:

							break;

						case CAR_FILENAME_TYPE:

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
				m_carFactory.front().second = true;
				m_checkpointMap->iterate(m_carFactory);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				m_userCar->rotate(0.0);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				m_userCar->rotate(1.0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				m_userCar->accelerate(1.0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				m_userCar->brake(1.0);
			}
			
			if (m_edgeManager)
			{
				m_userCar->update();
				m_edgeManager->Intersect(m_carFactory);

				// Update view
				auto& view = CoreWindow::GetView();
				view.setCenter(m_userCar->getCenter());
				CoreWindow::GetRenderWindow().setView(view);
			}

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
	// Set consistent texts
	m_modeText.SetConsistentText("Mode:");
	m_fitnessText.SetConsistentText("Highest fitness:");
	m_filenameTypeText.SetConsistentText("Filename type:");

	// Set variable texts
	m_textFunctions.push_back([&] { return m_modeStrings[m_mode]; });
	m_modeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return m_checkpointMap ? std::to_string(m_checkpointMap->getHighestFitness()) : "0"; });
	m_fitnessText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.05));
	m_textFunctions.push_back([&] { return m_filenameTypeStrings[m_filenameType]; });
	m_filenameTypeText.SetObserver(new FunctionTimerObserver<std::string>(m_textFunctions.back(), 0.2));

	// Set information texts
	m_modeText.SetInformationText("| [M]");
	m_fitnessText.SetInformationText("| [Enter]");
	m_filenameTypeText.SetInformationText("| [F]");
	
	// Set texts positions
	m_modeText.SetPosition({ FontContext::Component(0), {3}, {7}, {0} });
	m_fitnessText.SetPosition({ FontContext::Component(0), {4}, {7}, {1, true} });
	m_filenameTypeText.SetPosition({ FontContext::Component(0), {3}, {7}, {1} });
	m_filenameText.SetPosition({ FontContext::Component(0), {3}, {7}, {13}, {2} });

	CoreLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::Draw()
{
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
	
	for (auto& car : m_carFactory)
	{
		if (!car.second)
			continue;
		car.first->drawBody();
		car.first->drawBeams();
	}

	if (m_edgeManager)
	{
		m_edgeManager->Draw();
		m_checkpointMap->draw();
	}
	
	m_modeText.Draw();
	m_fitnessText.Draw();
}
