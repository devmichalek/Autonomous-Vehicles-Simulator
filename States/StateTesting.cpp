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
		{
			++m_numberOfVehicles;
			m_drawableVehicleFactory.resize(m_numberOfVehicles, nullptr);
			m_artificialNeuralNetworks.resize(m_numberOfVehicles, nullptr);
		}

		m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
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
			--m_currentVehicle;

		--m_numberOfVehicles;
		m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
		m_textObservers[NUMBER_OF_VEHICLES_TEXT]->Notify();
	}
}

std::string StateTesting::GetCurrentVehicleName()
{
	if (m_numberOfVehicles == 0)
		return "None";

	std::string data = " [ ][ ]";
	data[2] = m_drawableVehicleFactory[m_currentVehicle] ? 'X' : ' ';
	data[5] = m_artificialNeuralNetworks[m_currentVehicle] ? 'X' : ' ';
	return std::string("Vehicle ") + std::to_string(m_currentVehicle) + data;
}

StateTesting::StateTesting() :
	m_numberOfVehicles(0),
	m_currentVehicle(0),
	m_maxNumberOfVehicles(10),
	m_activateUserVehicle(false),
	m_showCheckpoints(false),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(500.0)
{
	m_modeStrings[STOPPED_MODE] = "Stopped";
	m_modeStrings[RUNNING_MODE] = "Running";
	m_modeStrings[PAUSED_MODE] = "Paused";
	m_mode = STOPPED_MODE;

	m_filenameTypeStrings[MAP_FILENAME_TYPE] = "Map";
	m_filenameTypeStrings[ANN_FILENAME_TYPE] = "ANN";
	m_filenameTypeStrings[VEHICLE_FILENAME_TYPE] = "Vehicle";
	m_filenameType = MAP_FILENAME_TYPE;

	m_parameterTypesStrings[SWITCH_VEHICLE] = "Switch vehicle";
	m_parameterTypesStrings[NUMBER_OF_VEHICLES] = "Number of vehicles";
	m_parameterTypesStrings[ACTIVATE_USER_VEHICLE] = "Activate user vehicle";
	m_parameterTypesStrings[SHOW_CHECKPOINTS] = "Show checkpoints";
	m_parameterType = SWITCH_VEHICLE;

	m_controlKeys[sf::Keyboard::M] = CHANGE_MODE;
	m_controlKeys[sf::Keyboard::F] = CHANGE_FILENAME_TYPE;
	m_controlKeys[sf::Keyboard::Tab] = SWITCH_VEHICLE;
	m_controlKeys[sf::Keyboard::Add] = INCREASE_PARAMETER;
	m_controlKeys[sf::Keyboard::Subtract] = DECREASE_PARAMETER;
	m_controlKeys[sf::Keyboard::P] = CHANGE_PARAMETER;

	for (auto& pressedKey : m_pressedKeys)
		pressedKey = false;

	m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED] = "Error: One of artificial neural network is not specified!";
	m_internalErrorsStrings[ERROR_NO_DRAWABLE_MAP_SPECIFIED] = "Error: No drawable map is specified!";
	m_internalErrorsStrings[ERROR_NO_DRAWABLE_VEHICLE_SPECIFIED] = "Error: One of drawable vehicle is not specified!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: One of artificial neural network number of input neurons mismatches number of vehicle sensors!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH] = "Error: One of artificial neural network number of output neurons mismatches number of vehicle (3) inputs!";
	m_internalErrorsStrings[ERROR_NO_DRAWABLE_VEHICLES] = "Error: There are no drawable vehicles! Please add first drawable vehicle to continue.";

	m_drawableMap = nullptr;

	// Initialize user vehicle with dummy vehicle
	m_drawableVehicleBuilder.CreateDummy();
	m_userVehicle = m_drawableVehicleBuilder.Get();
	m_dummyVehicle = m_drawableVehicleBuilder.Get();
	m_drawableVehicleBuilder.Clear();

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
	m_parameterType = SWITCH_VEHICLE;

	// Reset pressed keys
	for (size_t i = 0; i < m_pressedKeys.size(); ++i)
		m_pressedKeys[i] = false;

	// Reset testing parameters
	m_numberOfVehicles = 0;
	m_currentVehicle = 0;
	m_activateUserVehicle = false;
	m_showCheckpoints = false;
	m_viewTimer.Reset();

	// Reset objects of environment
	delete m_drawableMap;
	m_drawableMap = nullptr;
	for (auto& vehicle : m_drawableVehicleFactory)
	{
		delete vehicle;
		vehicle = nullptr;
	}
	m_drawableVehicleFactory.clear();
	for (auto& ann : m_artificialNeuralNetworks)
	{
		delete ann;
		ann = nullptr;
	}
	m_artificialNeuralNetworks.clear();

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
								m_mode = RUNNING_MODE;
								DrawableStatusText* modeText = static_cast<DrawableStatusText*>(m_texts[MODE_TEXT]);
								if (!m_drawableMap)
								{
									modeText->ShowStatusText();
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_DRAWABLE_MAP_SPECIFIED]);
									m_mode = STOPPED_MODE;
									break;
								}

								bool error = false;
								for (size_t i = 0; i < m_numberOfVehicles; ++i)
								{
									if (!m_drawableVehicleFactory[i])
									{
										modeText->ShowStatusText();
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_DRAWABLE_VEHICLE_SPECIFIED]);
										error = true;
										break;
									}

									m_drawableVehicleFactory[i]->SetAngle(m_dummyVehicle->GetAngle());
									m_drawableVehicleFactory[i]->SetCenter(m_dummyVehicle->GetCenter());
									m_drawableVehicleFactory[i]->SetActive();
									m_drawableVehicleFactory[i]->Update();

									if (!m_artificialNeuralNetworks[i])
									{
										modeText->ShowStatusText();
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
										error = true;
										break;
									}

									if (m_artificialNeuralNetworks[i]->GetNumberOfInputNeurons() != m_drawableVehicleFactory[i]->GetNumberOfOutputs())
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
										m_mode = STOPPED_MODE;
										break;
									}

									if (m_artificialNeuralNetworks[i]->GetNumberOfOutputNeurons() != m_drawableVehicleFactory[i]->GetNumberOfInputs())
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH]);
										m_mode = STOPPED_MODE;
										break;
									}
								}

								if (error)
								{
									m_mode = STOPPED_MODE;
									break;
								}

								// Init drawable map
								m_drawableMap->Init(m_numberOfVehicles, 0.0);

								// Prepare user vehicle
								m_userVehicle->SetAngle(m_dummyVehicle->GetAngle());
								m_userVehicle->SetCenter(m_dummyVehicle->GetCenter());
								m_userVehicle->SetActive();

								m_textObservers[MODE_TEXT]->Notify();
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
							case CHANGE_PARAMETER:
							{
								++m_parameterType;
								if (m_parameterType >= PARAMETERS_COUNT)
									m_parameterType = SWITCH_VEHICLE;
								m_textObservers[PARAMETER_TYPE_TEXT]->Notify();
								break;
							}
							case INCREASE_PARAMETER:
							{
								switch (m_parameterType)
								{
									case SWITCH_VEHICLE:
										++m_currentVehicle;
										if (m_currentVehicle >= m_numberOfVehicles)
											m_currentVehicle = 0;
										m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
										break;
									case NUMBER_OF_VEHICLES:
										OnAddVehicle();
										break;
									case ACTIVATE_USER_VEHICLE:
										m_activateUserVehicle = true;
										m_textObservers[IS_USER_VEHICLE_ACTIVE_TEXT]->Notify();
										break;
									case SHOW_CHECKPOINTS:
										m_showCheckpoints = true;
										m_textObservers[SHOW_CHECKPOINTS_TEXT]->Notify();
										break;
								}
								
								break;
							}
							case DECREASE_PARAMETER:
							{
								switch (m_parameterType)
								{
									case SWITCH_VEHICLE:
										if (m_currentVehicle == 0)
											m_currentVehicle = m_numberOfVehicles - 1;
										else
											--m_currentVehicle;
										m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
										break;
									case NUMBER_OF_VEHICLES:
										OnRemoveVehicle();
										break;
									case ACTIVATE_USER_VEHICLE:
										m_activateUserVehicle = false;
										m_textObservers[IS_USER_VEHICLE_ACTIVE_TEXT]->Notify();
										break;
									case SHOW_CHECKPOINTS:
										m_showCheckpoints = false;
										m_textObservers[SHOW_CHECKPOINTS_TEXT]->Notify();
										break;
								}

								break;
							}
						}
					}
				}

				break;
			}
			case RUNNING_MODE:
			case PAUSED_MODE:
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
							m_mode = STOPPED_MODE;

							// Reset vehicles
							for (size_t i = 0; i < m_numberOfVehicles; ++i)
							{
								m_drawableVehicleFactory[i]->SetAngle(m_dummyVehicle->GetAngle());
								m_drawableVehicleFactory[i]->SetCenter(m_dummyVehicle->GetCenter());
								m_drawableVehicleFactory[i]->SetActive();
								m_drawableVehicleFactory[i]->Update();
							}

							// Reset view so that the center is car starting position
							auto& view = CoreWindow::GetView();
							view.setCenter(m_dummyVehicle->GetCenter());
							CoreWindow::GetRenderWindow().setView(view);
							break;
						}
						case CHANGE_FILENAME_TYPE:
						case INCREASE_PARAMETER:
						case DECREASE_PARAMETER:
							break;
						case PAUSED_CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;

							if (m_mode == RUNNING_MODE)
								m_mode = PAUSED_MODE;
							else
								m_mode = RUNNING_MODE;
							m_textObservers[MODE_TEXT]->Notify();
							break;
						}
						default:
							break;
					}

					m_pressedKeys[iterator->second] = true;
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

						// Reset view so that the center is car starting position
						auto& view = CoreWindow::GetView();
						view.setCenter(m_dummyVehicle->GetCenter());
						CoreWindow::GetRenderWindow().setView(view);
						break;
					}
					case ANN_FILENAME_TYPE:
					{
						if (m_numberOfVehicles == 0)
						{
							filenameText->ShowStatusText();
							filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_DRAWABLE_VEHICLES]);
							break;
						}

						bool success = m_artificialNeuralNetworkBuilder.Load(filenameText->GetFilename());
						auto status = m_artificialNeuralNetworkBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Update specified artificial neural network
						delete m_artificialNeuralNetworks[m_currentVehicle];
						m_artificialNeuralNetworks[m_currentVehicle] = m_artificialNeuralNetworkBuilder.Get();
						m_artificialNeuralNetworks[m_currentVehicle]->SetFromRawData(m_artificialNeuralNetworkBuilder.GetRawNeuronData());
						m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
						break;
					}
					case VEHICLE_FILENAME_TYPE:
					{
						if (m_numberOfVehicles == 0)
						{
							filenameText->ShowStatusText();
							filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_DRAWABLE_VEHICLES]);
							break;
						}

						bool success = m_drawableVehicleBuilder.Load(filenameText->GetFilename());
						auto status = m_drawableVehicleBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Update specified vehicle
						delete m_drawableVehicleFactory[m_currentVehicle];
						m_drawableVehicleFactory[m_currentVehicle] = m_drawableVehicleBuilder.Get();
						m_drawableMapBuilder.UpdateVehicle(m_drawableVehicleFactory[m_currentVehicle]);
						m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
						break;
					}
					default:
						break;
				}
			}

			break;
		}
		case RUNNING_MODE:
		case PAUSED_MODE:
		{
			if (m_mode == RUNNING_MODE && m_numberOfVehicles)
			{
				for (size_t i = 0; i < m_numberOfVehicles; ++i)
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

			if (m_activateUserVehicle)
			{
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

				// Update view
				auto& view = CoreWindow::GetView();
				view.setCenter(m_userVehicle->GetCenter());
				CoreWindow::GetRenderWindow().setView(view);
			}
			else if (m_viewTimer.Update())
			{
				sf::Vector2f m_viewCenter;
				if (m_numberOfVehicles)
				{
					auto index = m_drawableMap->MarkLeader(m_drawableVehicleFactory);
					m_viewCenter = m_drawableVehicleFactory[index]->GetCenter();
				}
				else
					m_viewCenter = m_dummyVehicle->GetCenter();

				// Update view
				auto& view = CoreWindow::GetView();
				auto currentViewCenter = view.getCenter();
				auto distance = DrawableMath::Distance(currentViewCenter, m_viewCenter);
				auto angle = DrawableMath::DifferenceVectorAngle(currentViewCenter, m_viewCenter);
				auto newCenter = DrawableMath::GetEndPoint(currentViewCenter, angle, float(-distance / m_viewMovementOffset));
				view.setCenter(newCenter);
				CoreWindow::GetRenderWindow().setView(view);
			}
			
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
	m_texts[MODE_TEXT] = new DrawableStatusText({ "Mode:", "", "| [M] [P]" });
	m_texts[FILENAME_TYPE_TEXT] = new DrawableTripleText({ "Filename type:", "", "| [F]" });
	m_texts[FILENAME_TEXT] = new DrawableFilenameText<true, false>("map.bin");
	m_texts[PARAMETER_TYPE_TEXT] = new DrawableTripleText({ "Parameter type:", "", "| [P] [+] [-]" });
	m_texts[CURRENT_VEHICLE_TEXT] = new DrawableDoubleText({ "Current vehicle:" });
	m_texts[NUMBER_OF_VEHICLES_TEXT] = new DrawableDoubleText({ "Number of vehicles:" });
	m_texts[SHOW_CHECKPOINTS_TEXT] = new DrawableDoubleText({ "Show checkpoints:" });
	m_texts[IS_USER_VEHICLE_ACTIVE_TEXT] = new DrawableDoubleText({ "Is user vehicle active:" });
	m_texts[USER_FITNESS_TEXT] = new DrawableDoubleText({ "User fitness:" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_modeStrings[m_mode]; }, 0.05);
	m_textObservers[FILENAME_TYPE_TEXT] = new FunctionTimerObserver<std::string>([&] { return m_filenameTypeStrings[m_filenameType]; }, 0.2);
	m_textObservers[PARAMETER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_parameterTypesStrings[m_parameterType]; });
	m_textObservers[CURRENT_VEHICLE_TEXT] = new FunctionEventObserver<std::string>([&] { return GetCurrentVehicleName(); });
	m_textObservers[NUMBER_OF_VEHICLES_TEXT] = new TypeEventObserver<size_t>(m_numberOfVehicles);
	m_textObservers[SHOW_CHECKPOINTS_TEXT] = new FunctionEventObserver<std::string>([&] { return m_showCheckpoints ? "True" : "False"; });
	m_textObservers[IS_USER_VEHICLE_ACTIVE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_activateUserVehicle ? "True" : "False"; });
	m_textObservers[USER_FITNESS_TEXT] = new FunctionTimerObserver<std::string>([&]{ return !m_drawableMap ? "Unknown" : std::to_string(m_drawableMap->CalculateFitness(m_userVehicle)); }, 0.5);

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {4}, {8}, {14} });
	m_texts[FILENAME_TYPE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {4}, {8} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {4}, {8}, {14} });
	m_texts[PARAMETER_TYPE_TEXT]->SetPosition({ FontContext::Component(5, true), {0}, {5}, {10} });
	m_texts[CURRENT_VEHICLE_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {5} });
	m_texts[NUMBER_OF_VEHICLES_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {5} });
	m_texts[SHOW_CHECKPOINTS_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {5} });
	m_texts[IS_USER_VEHICLE_ACTIVE_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {5} });
	m_texts[USER_FITNESS_TEXT]->SetPosition({ FontContext::Component(1, true), {7}, {11} });

	CoreLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::Draw()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			for (size_t i = 0; i < m_numberOfVehicles; ++i)
			{
				if (!m_drawableVehicleFactory[i])
					continue;
				m_drawableVehicleFactory[i]->DrawBody();
				if (!m_drawableVehicleFactory[i]->IsActive())
					continue;
				m_drawableVehicleFactory[i]->DrawBeams();
			}

			if (m_drawableMap)
			{
				m_drawableMap->Draw();
				if (m_showCheckpoints)
					m_drawableMap->DrawCheckpoints();
			}

			m_texts[FILENAME_TYPE_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			m_texts[PARAMETER_TYPE_TEXT]->Draw();
			m_texts[CURRENT_VEHICLE_TEXT]->Draw();
			m_texts[NUMBER_OF_VEHICLES_TEXT]->Draw();
			break;
		}
		case RUNNING_MODE:
		case PAUSED_MODE:
		{
			if (m_numberOfVehicles)
			{
				for (size_t i = 0; i < m_numberOfVehicles; ++i)
				{
					m_drawableVehicleFactory[i]->DrawBody();
					if (!m_drawableVehicleFactory[i]->IsActive())
						continue;
					m_drawableVehicleFactory[i]->DrawBeams();
				}
			}

			if (m_activateUserVehicle)
				m_userVehicle->DrawBody();
			
			m_drawableMap->Draw();
			if (m_showCheckpoints)
				m_drawableMap->DrawCheckpoints();

			if (m_activateUserVehicle)
				m_texts[USER_FITNESS_TEXT]->Draw();
			break;
		}
		default:
			break;
	}
	
	m_texts[MODE_TEXT]->Draw();
	m_texts[SHOW_CHECKPOINTS_TEXT]->Draw();
	m_texts[IS_USER_VEHICLE_ACTIVE_TEXT]->Draw();
}
