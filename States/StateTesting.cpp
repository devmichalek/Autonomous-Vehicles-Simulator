#pragma once
#include "StateTesting.hpp"
#include "CoreLogger.hpp"
#include "FunctionTimerObserver.hpp"
#include "DrawableFilenameText.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionEventObserver.hpp"

void StateTesting::OnAddVehicle()
{
	if (m_numberOfVehicles < m_maxNumberOfVehicles)
	{
		if (m_numberOfVehicles)
		{
			DrawableVehicleFactory drawableVehicleFactoryTemp(m_numberOfVehicles + 1, nullptr);
			ArtificialNeuralNetworks artificialNeuralNetworksTemp(m_numberOfVehicles + 1, nullptr);

			for (size_t i = 0; i <= m_currentVehicle; ++i)
			{
				drawableVehicleFactoryTemp[i] = m_drawableVehicleFactory[i];
				artificialNeuralNetworksTemp[i] = m_artificialNeuralNetworks[i];
			}

			drawableVehicleFactoryTemp[m_currentVehicle + 1] = nullptr;
			artificialNeuralNetworksTemp[m_currentVehicle + 1] = nullptr;

			for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
			{
				drawableVehicleFactoryTemp[i + 1] = m_drawableVehicleFactory[i];
				artificialNeuralNetworksTemp[i + 1] = m_artificialNeuralNetworks[i];
			}

			m_drawableVehicleFactory = drawableVehicleFactoryTemp;
			m_artificialNeuralNetworks = artificialNeuralNetworksTemp;
			++m_numberOfVehicles;
		}
		else
			++m_numberOfVehicles;

		m_textObservers[NUMBER_OF_VEHICLES_TEXT]->Notify();
	}
}

void StateTesting::OnRemoveVehicle()
{
	if (m_numberOfVehicles)
	{
		delete m_drawableVehicleFactory[m_currentVehicle];
		delete m_artificialNeuralNetworks[m_currentVehicle];

		for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
		{
			m_drawableVehicleFactory[i - 1] = m_drawableVehicleFactory[i];
			m_artificialNeuralNetworks[i - 1] = m_artificialNeuralNetworks[i];
		}

		m_drawableVehicleFactory[m_numberOfVehicles - 1] = nullptr;
		m_artificialNeuralNetworks[m_numberOfVehicles - 1] = nullptr;

		if (m_currentVehicle)
		{
			--m_currentVehicle;
			m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
		}

		--m_numberOfVehicles;
		m_textObservers[NUMBER_OF_VEHICLES_TEXT]->Notify();
	}
}

StateTesting::StateTesting() :
	m_numberOfVehicles(0),
	m_currentVehicle(0),
	m_maxNumberOfVehicles(10),
	m_lastUserCalculatedFitness(0)
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_mode = STOPPED_MODE;

	m_filenameTypeStrings[MAP_FILENAME_TYPE] = "Map";
	m_filenameTypeStrings[ANN_FILENAME_TYPE] = "ANN";
	m_filenameTypeStrings[VEHICLE_FILENAME_TYPE] = "Vehicle";
	m_filenameType = MAP_FILENAME_TYPE;

	m_controlKeys[sf::Keyboard::M] = CHANGE_MODE;
	m_controlKeys[sf::Keyboard::F] = CHANGE_FILENAME_TYPE;
	m_controlKeys[sf::Keyboard::Tab] = SWITCH_VEHICLE;
	m_controlKeys[sf::Keyboard::Add] = ADD_VEHICLE;
	m_controlKeys[sf::Keyboard::Subtract] = REMOVE_VEHICLE;

	for (auto& pressedKey : m_pressedKeys)
		pressedKey = false;

	m_internalErrorsStrings[NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED] = "Error: One of artificial neural network is not specified!";
	m_internalErrorsStrings[NO_DRAWABLE_MAP_SPECIFIED] = "Error: No drawable map is specified!";
	m_internalErrorsStrings[NO_DRAWABLE_VEHICLE_SPECIFIED] = "Error: One of drawable vehicle is not specified!";
	m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: One of artificial neural network number of input neurons mismatches number of vehicle sensors!";
	m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH] = "Error: One of artificial neural network number of output neurons mismatches number of vehicle (3) inputs!";

	m_drawableMap = nullptr;

	// Initialize user vehicle with dummy vehicle
	m_drawableVehicleBuilder.CreateDummy();
	m_userVehicle = m_drawableVehicleBuilder.Get();
	m_dummyVehicle = m_drawableVehicleBuilder.Get();
	m_drawableVehicleBuilder.Clear();

	m_drawableVehicleFactory.resize(m_maxNumberOfVehicles);
	m_artificialNeuralNetworks.resize(m_maxNumberOfVehicles);

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateTesting::~StateTesting()
{
	delete m_drawableMap;
	delete m_userVehicle;
	delete m_dummyVehicle;
	for (auto& vehicle : m_drawableVehicleFactory)
		delete vehicle;
	for (auto& ann : m_artificialNeuralNetworks)
		delete ann;
	for (auto& text : m_texts)
		delete text;
	for (auto& observer : m_textObservers)
		delete observer;
}

void StateTesting::Reload()
{
	// Reset states
	m_mode = STOPPED_MODE;
	m_filenameType = MAP_FILENAME_TYPE;

	// Reset pressed keys
	for (size_t i = 0; i < m_pressedKeys.size(); ++i)
		m_pressedKeys[i] = false;

	// Reset testing parameters
	m_numberOfVehicles = 0;
	m_currentVehicle = 0;
	m_lastUserCalculatedFitness = 0;

	// Reset objects of environment
	delete m_drawableMap;
	m_drawableMap = nullptr;
	for (auto& vehicle : m_drawableVehicleFactory)
	{
		delete vehicle;
		vehicle = nullptr;
	}
	for (auto& ann : m_artificialNeuralNetworks)
	{
		delete ann;
		ann = nullptr;
	}

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
	if (!filenameText->IsRenaming())
	{
		switch (m_mode)
		{
			case STOPPED_MODE:
			{
				filenameText->Capture();
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					auto eventKey = CoreWindow::GetEvent().key.code;
					auto iterator = m_controlKeys.find(eventKey);
					if (iterator != m_controlKeys.end() && !m_pressedKeys[iterator->second])
					{
						m_pressedKeys[iterator->second] = true;
						switch (iterator->second)
						{
							case CHANGE_MODE:
							{
								++m_mode;
								if (m_mode >= MODES_COUNT)
									m_mode = STOPPED_MODE;
								else if (m_mode == RUNNING_MODE)
								{
									DrawableStatusText* activeModeText = static_cast<DrawableStatusText*>(m_texts[ACTIVE_MODE_TEXT]);
									if (!m_drawableMap)
									{
										activeModeText->ShowStatusText();
										activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_DRAWABLE_MAP_SPECIFIED]);
										m_mode = STOPPED_MODE;
										break;
									}

									bool error = false;
									for (size_t i = 0; i < m_numberOfVehicles; ++i)
									{
										if (!m_drawableVehicleFactory[i])
										{
											activeModeText->ShowStatusText();
											activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_DRAWABLE_VEHICLE_SPECIFIED]);
											error = true;
											break;
										}

										m_drawableVehicleFactory[i]->SetAngle(m_dummyVehicle->GetAngle());
										m_drawableVehicleFactory[i]->SetCenter(m_dummyVehicle->GetCenter());
										
										if (!m_artificialNeuralNetworks[i])
										{
											activeModeText->ShowStatusText();
											activeModeText->SetErrorStatusText(m_internalErrorsStrings[NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
											error = true;
											break;
										}

										if (m_artificialNeuralNetworks[i]->GetNumberOfInputNeurons() != m_drawableVehicleFactory[i]->GetNumberOfOutputs())
										{
											activeModeText->SetErrorStatusText(m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
											m_mode = STOPPED_MODE;
											break;
										}

										if (m_artificialNeuralNetworks[i]->GetNumberOfOutputNeurons() != m_drawableVehicleFactory[i]->GetNumberOfInputs())
										{
											activeModeText->SetErrorStatusText(m_internalErrorsStrings[ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH]);
											m_mode = STOPPED_MODE;
											break;
										}
									}

									if (error)
									{
										m_mode = STOPPED_MODE;
										break;
									}

									m_userVehicle->SetAngle(m_dummyVehicle->GetAngle());
									m_userVehicle->SetCenter(m_dummyVehicle->GetCenter());
									m_userVehicle->SetActive();
								}

								m_textObservers[ACTIVE_MODE_TEXT]->Notify();
								break;
							}
							case CHANGE_FILENAME_TYPE:
							{
								++m_filenameType;
								if (m_filenameType >= FILENAME_TYPES_COUNT)
									m_filenameType = MAP_FILENAME_TYPE;
								m_textObservers[FILENAME_TYPE_TEXT]->Notify();
								break;
							}
							case SWITCH_VEHICLE:
							{
								++m_currentVehicle;
								if (m_currentVehicle >= m_numberOfVehicles)
									m_currentVehicle = 0;
								m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
								break;
							}
							case ADD_VEHICLE:
							{
								OnAddVehicle();
								break;
							}
							case REMOVE_VEHICLE:
							{
								OnRemoveVehicle();
								break;
							}
						}
					}
				}

				break;
			}
			case RUNNING_MODE:
			{
				if (CoreWindow::GetEvent().type == sf::Event::KeyPressed)
				{
					auto eventKey = CoreWindow::GetEvent().key.code;
					auto iterator = m_controlKeys.find(eventKey);
					if (iterator == m_controlKeys.end())
						break;

					switch (iterator->second)
					{
						case CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;
							m_pressedKeys[iterator->second] = true;
							++m_mode;
							if (m_mode >= MODES_COUNT)
								m_mode = STOPPED_MODE;

							// Reset view
							auto& view = CoreWindow::GetView();
							auto viewOffset = CoreWindow::GetViewOffset();
							view.move(-viewOffset);
							CoreWindow::GetRenderWindow().setView(view);
							break;
						}
						case CHANGE_FILENAME_TYPE:
						default:
							break;
					}
				}
				break;
			}
			default:
				break;
		}
	}
	else
		filenameText->Capture();


	if (CoreWindow::GetEvent().type == sf::Event::KeyReleased)
	{
		auto eventKey = CoreWindow::GetEvent().key.code;
		auto iterator = m_controlKeys.find(eventKey);
		if (iterator != m_controlKeys.end())
			m_pressedKeys[iterator->second] = false;
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
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Prepare drawable map
						delete m_drawableMap;
						m_drawableMap = m_drawableMapBuilder.Get();
						m_drawableMap->Init(0, 0.0);

						// Prepare dummy vehicle
						m_drawableMapBuilder.UpdateVehicle(m_dummyVehicle);
						break;
					}
					case ANN_FILENAME_TYPE:
					{

						break;
					}
					case VEHICLE_FILENAME_TYPE:
					{

						break;
					}
					default:
						break;
				}
			}

			break;
		}
		case RUNNING_MODE:
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				m_lastUserCalculatedFitness = m_drawableMap->CalculateFitness(m_userVehicle);
				m_textObservers[USER_FITNESS]->Notify();
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

			// Update user vehicle
			m_userVehicle->Update();

			if (m_drawableVehicleFactory.front())
			{
				for (size_t i = 0; i < m_drawableVehicleFactory.size(); ++i)
				{
					if (!m_drawableVehicleFactory[i]->IsActive())
						continue;

					const NeuronLayer& input = m_drawableVehicleFactory[i]->ProcessOutput();
					const NeuronLayer& output = m_artificialNeuralNetworks[i]->Update(input);
					m_drawableVehicleFactory[i]->ProcessInput(output);
					m_drawableVehicleFactory[i]->Update();
				}

				if (m_drawableMap)
				{
					// Check intersection
					m_drawableMap->Intersect(m_drawableVehicleFactory);
				}
			}

			// Update view
			auto& view = CoreWindow::GetView();
			view.setCenter(m_userVehicle->GetCenter());
			CoreWindow::GetRenderWindow().setView(view);
			break;
		}
		default:
			break;
	}

	for (auto& text : m_texts)
		text->Update();
}

bool StateTesting::Load()
{
	// Create texts
	m_texts[ACTIVE_MODE_TEXT] = new DrawableStatusText({ "Mode:", "", "| [M]" });
	m_texts[FILENAME_TYPE_TEXT] = new DrawableTripleText({ "Filename type:", "", "| [F]" });
	m_texts[FILENAME_TEXT] = new DrawableFilenameText<true, false>;
	m_texts[NUMBER_OF_VEHICLES_TEXT] = new DrawableTripleText({ "Number of vehicles:", "", "| [+] [-]" });
	m_texts[CURRENT_VEHICLE_TEXT] = new DrawableTripleText({ "Current vehicle:", "", "| [Tab]" });
	m_texts[USER_FITNESS] = new DrawableTripleText({ "User fitness:", "", "| [Enter]" });

	// Create observers
	m_textObservers[ACTIVE_MODE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_modeStrings[m_mode]; }, 0.05);
	m_textObservers[FILENAME_TYPE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_filenameTypeStrings[m_filenameType]; }, 0.2);
	m_textObservers[NUMBER_OF_VEHICLES_TEXT] = new TypeEventObserver<size_t>(m_numberOfVehicles);
	m_textObservers[CURRENT_VEHICLE_TEXT] = new FunctionEventObserver<std::string>([&] { return std::string("Vehicle ") + std::to_string(m_currentVehicle); });
	m_textObservers[USER_FITNESS] = new TypeEventObserver<Fitness>(m_lastUserCalculatedFitness);

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[ACTIVE_MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {4}, {8}, {14} });
	m_texts[FILENAME_TYPE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {4}, {8} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {4}, {8}, {14} });
	m_texts[NUMBER_OF_VEHICLES_TEXT]->SetPosition({ FontContext::Component(3), {0}, {4}, {8} });
	m_texts[CURRENT_VEHICLE_TEXT]->SetPosition({ FontContext::Component(4), {0}, {4}, {8} });
	m_texts[USER_FITNESS]->SetPosition({ FontContext::Component(1), {0}, {4}, {8} });

	CoreLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::Draw()
{
	if (m_drawableVehicleFactory.front())
	{
		for (auto& vehicle : m_drawableVehicleFactory)
		{
			vehicle->DrawBody();
			if (!vehicle->IsActive())
				continue;
			vehicle->DrawBeams();
		}
	}

	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			if (m_drawableMap)
			{
				m_drawableMap->Draw();
				m_drawableMap->DrawDebug();
			}

			m_texts[FILENAME_TYPE_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			m_texts[NUMBER_OF_VEHICLES_TEXT]->Draw();
			m_texts[CURRENT_VEHICLE_TEXT]->Draw();
			break;
		}
		case RUNNING_MODE:
		{
			m_userVehicle->DrawBody();
			m_userVehicle->DrawBeams();

			m_drawableMap->Draw();
			m_drawableMap->DrawDebug();

			m_texts[USER_FITNESS]->Draw();
			break;
		}
		default:
			break;
	}
	
	m_texts[ACTIVE_MODE_TEXT]->Draw();
}
