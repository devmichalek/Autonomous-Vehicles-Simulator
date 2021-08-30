#pragma once
#include "StateTesting.hpp"
#include "CoreLogger.hpp"
#include "FunctionTimerObserver.hpp"
#include "DrawableFilenameText.hpp"

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
	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateTesting::~StateTesting()
{
	delete m_drawableMap;
	delete m_userVehicle;
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateTesting::Reload()
{
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;
	m_modeKey.second = false;
	m_filenameTypeKey.second = false;

	// Delete objects of test
	delete m_drawableMap;
	m_drawableMap = nullptr;
	delete m_userVehicle;
	m_userVehicle = nullptr;

	// Clear builders
	m_drawableMapBuilder.Clear();
	m_drawableVehicleBuilder.Clear();
	m_artificialNeuralNetworkBuilder.Clear();

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

void StateTesting::Capture()
{
	auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
	if (filenameText->IsRenaming())
		filenameText->Capture();
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
						filenameText->Capture();

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
						filenameText->Capture();
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
			auto* filenameText = static_cast<DrawableFilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
			if (filenameText->IsReading())
			{
				switch (m_filenameType)
				{
					case MAP_FILENAME_TYPE:
					{
						bool success = m_drawableMapBuilder.Load(filenameText->GetFilename());
						auto status = m_drawableMapBuilder.GetLastOperationStatus();
						filenameText->ShowStatusText();
						if (success)
						{
							filenameText->SetSuccessStatusText(status.second);
							delete m_drawableMap;
							m_drawableMap = m_drawableMapBuilder.Get();
							m_drawableMap->Init(m_drawableVehicleFactory.size(), 0.0);

							for (auto& vehicle : m_drawableVehicleFactory)
							{
								delete vehicle;
								vehicle = m_drawableVehicleBuilder.Get();
								m_drawableMapBuilder.UpdateVehicle(vehicle);
							}

							m_userVehicle = m_drawableVehicleFactory[0];

							// Update view
							auto& view = CoreWindow::GetView();
							view.setCenter(m_userVehicle->GetCenter());
							CoreWindow::GetRenderWindow().setView(view);
						}
						else
							filenameText->SetErrorStatusText(status.second);
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
			else if (!filenameText->IsRenaming())
			{
				if (m_filenameTypeKey.second)
				{
					++m_filenameType;
					if (m_filenameType >= FILENAME_TYPES_COUNT)
						m_filenameType = MAP_FILENAME_TYPE;
					m_filenameTypeKey.second = false;
				}
			}

			break;
		}
		case RUNNING_MODE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				m_drawableVehicleFactory.front()->SetActive();
				m_drawableMap->Iterate(m_drawableVehicleFactory);
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

	for (auto& text : m_texts)
		text->Update();
}

bool StateTesting::Load()
{
	// Create texts
	m_texts[ACTIVE_MODE_TEXT] = new DrawableTripleText({ "Mode:", "", "| [M]" });
	m_texts[FITNESS_TEXT] = new DrawableTripleText({ "Highest fitness:", "", "| [Enter]" });
	m_texts[FILENAME_TYPE_TEXT] = new DrawableTripleText({ "Filename type:", "", "| [F]" });
	m_texts[FILENAME_TEXT] = new DrawableFilenameText<true, false>;

	// Create observers
	m_textObservers[ACTIVE_MODE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_modeStrings[m_mode]; }, 0.05);
	m_textObservers[FITNESS_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_drawableMap ? std::to_string(m_drawableMap->GetHighestFitness()) : "0"; }, 0.05);
	m_textObservers[FILENAME_TYPE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_filenameTypeStrings[m_filenameType]; }, 0.2);

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[ACTIVE_MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {7} });
	m_texts[FITNESS_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {4}, {7} });
	m_texts[FILENAME_TYPE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {7} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {7}, {13} });

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
		m_drawableMap->DrawDebug();
	}

	switch (m_mode)
	{
		case STOPPED_MODE:
			m_texts[FILENAME_TYPE_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			break;
		case RUNNING_MODE:
			break;
		default:
			break;
	}
	
	m_texts[ACTIVE_MODE_TEXT]->Draw();
	m_texts[FITNESS_TEXT]->Draw();
}
