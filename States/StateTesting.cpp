#pragma once
#include "StateTesting.hpp"
#include "CoreLogger.hpp"
#include "FilenameText.hpp"
#include "ArtificialNeuralNetwork.hpp"
#include "TypeEventObserver.hpp"
#include "FunctionEventObserver.hpp"
#include "FunctionTimerObserver.hpp"
#include "SimulatedWorld.hpp"
#include "MapPrototype.hpp"
#include "FitnessSystem.hpp"
#include "DrawableCheckpoint.hpp"

StateTesting::StateTesting() :
	m_numberOfVehicles(0),
	m_currentVehicle(0),
	m_maxNumberOfVehicles(10),
	m_enableUserVehicle(false, true, true, false),
	m_enableCheckpoints(false, true, true, false),
	m_zoom(1.f, 4.f, 0.3f, 1.f),
	m_viewTimer(1.0, 0.1),
	m_viewMovementOffset(3.0),
	m_pressedKeyTimer(0.0, 1.0, 5000)
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
	m_parameterTypesStrings[ENABLE_USER_VEHICLE] = "Enable user vehicle";
	m_parameterTypesStrings[ENABLE_CHECKPOINTS] = "Enable checkpoints";
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
	m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED] = "Error: No map is specified!";
	m_internalErrorsStrings[ERROR_NO_VEHICLE_SPECIFIED] = "Error: One of vehicles is not fully specified!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH] = "Error: One of artificial neural network number of input neurons mismatches number of vehicle sensors!";
	m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH] = "Error: One of artificial neural network number of output neurons mismatches number of vehicle (3) inputs!";
	m_internalErrorsStrings[ERROR_NO_VEHICLES] = "Error: There are no vehicles! Please add first vehicle to continue.";

	m_pressedKeyTimer.MakeTimeout();
	m_simulatedWorld = nullptr;
	m_fitnessSystem = nullptr;
	m_mapPrototype = nullptr;

	// Initialize user vehicle with dummy vehicle
	m_vehicleBuilder.CreateDummy();
	m_userVehicle = nullptr;
	m_userVehiclePrototype = m_vehicleBuilder.Get();
	m_vehicleBuilder.Clear();

	m_texts.resize(TEXT_COUNT, nullptr);
	m_textObservers.resize(TEXT_COUNT, nullptr);
}

StateTesting::~StateTesting()
{
	delete m_simulatedWorld;
	delete m_fitnessSystem;
	delete m_mapPrototype;
	delete m_userVehiclePrototype;
	for (auto& vehiclePrototype : m_vehiclePrototypes)
		delete vehiclePrototype;
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
	m_enableUserVehicle.ResetValue();
	m_enableCheckpoints.ResetValue();
	m_zoom.ResetValue();
	m_viewTimer.Reset();
	m_pressedKeyTimer.MakeTimeout();

	// Reset objects of environment
	delete m_simulatedWorld;
	m_simulatedWorld = nullptr;
	delete m_fitnessSystem;
	m_fitnessSystem = nullptr;
	delete m_mapPrototype;
	m_mapPrototype = nullptr;
	m_simulatedVehicles.clear();
	for (auto& vehiclePrototype : m_vehiclePrototypes)
	{
		delete vehiclePrototype;
		vehiclePrototype = nullptr;
	}
	m_vehiclePrototypes.clear();
	for (auto& ann : m_artificialNeuralNetworks)
	{
		delete ann;
		ann = nullptr;
	}
	m_artificialNeuralNetworks.clear();

	// Clear builders
	m_mapBuilder.Clear();
	m_vehicleBuilder.Clear();
	m_artificialNeuralNetworkBuilder.Clear();

	// Reset texts and text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
	{
		if (m_textObservers[i])
			m_textObservers[i]->Notify();
		m_texts[i]->Reset();
	}

	CoreWindow::Reset();
}

void StateTesting::Capture()
{
	auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
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
								StatusText* modeText = static_cast<StatusText*>(m_texts[MODE_TEXT]);
								if (!m_mapPrototype)
								{
									modeText->ShowStatusText();
									modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_MAP_SPECIFIED]);
									break;
								}

								bool error = false;
								for (size_t i = 0; i < m_numberOfVehicles; ++i)
								{
									if (!m_vehiclePrototypes[i])
									{
										modeText->ShowStatusText();
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_VEHICLE_SPECIFIED]);
										error = true;
										break;
									}

									m_vehiclePrototypes[i]->SetAngle(m_userVehiclePrototype->GetAngle());
									m_vehiclePrototypes[i]->SetCenter(m_userVehiclePrototype->GetCenter());

									if (!m_artificialNeuralNetworks[i])
									{
										modeText->ShowStatusText();
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_ARTIFICIAL_NEURAL_NETWORK_SPECIFIED]);
										error = true;
										break;
									}

									if (m_artificialNeuralNetworks[i]->GetNumberOfInputNeurons() != m_vehiclePrototypes[i]->GetNumberOfSensors())
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_INPUT_MISMATCH]);
										break;
									}

									if (m_artificialNeuralNetworks[i]->GetNumberOfOutputNeurons() != VehicleBuilder::GetDefaultNumberOfInputs())
									{
										modeText->SetErrorStatusText(m_internalErrorsStrings[ERROR_ARTIFICIAL_NEURAL_NETWORK_OUTPUT_MISMATCH]);
										break;
									}
								}

								if (error)
									break;

								// Init simulated world
								delete m_simulatedWorld;
								m_simulatedWorld = new SimulatedWorld;
								m_simulatedWorld->AddMap(m_mapPrototype);
								DrawableCheckpoint::SetVisibility(m_enableCheckpoints);
								delete m_fitnessSystem;
								m_fitnessSystem = new FitnessSystem(m_numberOfVehicles, m_mapPrototype->GetNumberOfCheckpoints(), 0.0);
								m_simulatedWorld->AddBeginContactFunction(m_fitnessSystem->GetBeginContactFunction());

								// Prepare user vehicle
								if (m_enableUserVehicle)
									m_userVehicle = m_simulatedWorld->AddVehicle(m_userVehiclePrototype);

								// Add bot vehicles to the world
								m_simulatedVehicles.resize(m_numberOfVehicles, nullptr);
								for (size_t i = 0; i < m_numberOfVehicles; ++i)
									m_simulatedVehicles[i] = m_simulatedWorld->AddVehicle(m_vehiclePrototypes[i]);

								m_mode = RUNNING_MODE;
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
										if (m_numberOfVehicles > 1)
										{
											++m_currentVehicle;
											if (m_currentVehicle >= m_numberOfVehicles)
												m_currentVehicle = 0;
											m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
										}
										break;
									case NUMBER_OF_VEHICLES:
										OnAddVehicle();
										break;
									case ENABLE_USER_VEHICLE:
										if (!m_enableUserVehicle)
										{
											m_enableUserVehicle.Increase();
											m_textObservers[ENABLE_USER_VEHICLE_TEXT]->Notify();
										}
										break;
									case ENABLE_CHECKPOINTS:
										if (!m_enableCheckpoints)
										{
											m_enableCheckpoints.Increase();
											m_textObservers[ENABLE_CHECKPOINTS_TEXT]->Notify();
										}
										break;
								}
								
								break;
							}
							case DECREASE_PARAMETER:
							{
								switch (m_parameterType)
								{
									case SWITCH_VEHICLE:
										if (m_numberOfVehicles > 1)
										{
											if (m_currentVehicle == 0)
												m_currentVehicle = m_numberOfVehicles - 1;
											else
												--m_currentVehicle;
											m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
										}
										break;
									case NUMBER_OF_VEHICLES:
										OnRemoveVehicle();
										break;
									case ENABLE_USER_VEHICLE:
										if (m_enableUserVehicle)
										{
											m_enableUserVehicle.Decrease();
											m_textObservers[ENABLE_USER_VEHICLE_TEXT]->Notify();
										}
										break;
									case ENABLE_CHECKPOINTS:
										if (m_enableCheckpoints)
										{
											m_enableCheckpoints.Decrease();
											m_textObservers[ENABLE_CHECKPOINTS_TEXT]->Notify();
										}
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
							m_textObservers[MODE_TEXT]->Notify();

							// Reset vehicles
							for (auto& vehiclePrototype : m_vehiclePrototypes)
							{
								vehiclePrototype->SetAngle(m_userVehiclePrototype->GetAngle());
								vehiclePrototype->SetCenter(m_userVehiclePrototype->GetCenter());
							}

							// Reset view so that the center is vehicle starting position
							m_zoom.ResetValue();
							m_textObservers[ZOOM_TEXT]->Notify();
							CoreWindow::Reset();
							CoreWindow::SetViewCenter(m_userVehiclePrototype->GetCenter());
							break;
						}
						case CHANGE_FILENAME_TYPE:
							break;
						case INCREASE_PARAMETER:
							if (m_pressedKeyTimer.Update())
							{
								m_zoom.Decrease();
								CoreWindow::SetViewZoom(m_zoom);
								m_textObservers[ZOOM_TEXT]->Notify();
							}
							break;
						case DECREASE_PARAMETER:
							if (m_pressedKeyTimer.Update())
							{
								m_zoom.Increase();
								CoreWindow::SetViewZoom(m_zoom);
								m_textObservers[ZOOM_TEXT]->Notify();
							}
							break;
						case PAUSED_CHANGE_MODE:
						{
							if (m_pressedKeys[iterator->second])
								break;
							m_mode = PAUSED_MODE;
							m_textObservers[MODE_TEXT]->Notify();
							break;
						}
						default:
							break;
					}

					m_pressedKeys[iterator->second] = true;
				}
				break;
			case PAUSED_MODE:
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
							m_textObservers[MODE_TEXT]->Notify();

							// Reset vehicles
							for (auto &vehiclePrototype : m_vehiclePrototypes)
							{
								vehiclePrototype->SetAngle(m_userVehiclePrototype->GetAngle());
								vehiclePrototype->SetCenter(m_userVehiclePrototype->GetCenter());
							}

							// Reset view so that the center is vehicle starting position
							m_zoom.ResetValue();
							m_textObservers[ZOOM_TEXT]->Notify();
							CoreWindow::Reset();
							CoreWindow::SetViewCenter(m_userVehiclePrototype->GetCenter());
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
		{
			m_pressedKeys[iterator->second] = false;
			switch (iterator->second)
			{
				case CHANGE_MODE:
				case CHANGE_FILENAME_TYPE:
				case CHANGE_PARAMETER:
					break;
				case INCREASE_PARAMETER:
				case DECREASE_PARAMETER:
					m_pressedKeyTimer.MakeTimeout();
					break;
				default:
					break;
			}
		}
	}
}

void StateTesting::Update()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
		{
			auto* filenameText = static_cast<FilenameText<true, true>*>(m_texts[FILENAME_TEXT]);
			if (filenameText->IsReading())
			{
				switch (m_filenameType)
				{
					case MAP_FILENAME_TYPE:
					{
						bool success = m_mapBuilder.Load(filenameText->GetFilename());
						auto status = m_mapBuilder.GetLastOperationStatus();
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Prepare map prototype
						delete m_mapPrototype;
						m_mapPrototype = m_mapBuilder.Get();
						DrawableCheckpoint::SetVisibility(m_enableCheckpoints);

						// Prepare dummy vehicle
						m_userVehiclePrototype->SetCenter(m_mapBuilder.GetVehicleCenter());
						m_userVehiclePrototype->SetAngle(m_mapBuilder.GetVehicleAngle());
						m_userVehiclePrototype->Update();

						// Reset view so that the center is vehicle starting position
						CoreWindow::SetViewCenter(m_userVehiclePrototype->GetCenter());
						break;
					}
					case ANN_FILENAME_TYPE:
					{
						if (m_numberOfVehicles == 0)
						{
							filenameText->ShowStatusText();
							filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_VEHICLES]);
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
							filenameText->SetErrorStatusText(m_internalErrorsStrings[ERROR_NO_VEHICLES]);
							break;
						}

						bool success = m_vehicleBuilder.Load(filenameText->GetFilename());
						auto status = m_vehicleBuilder.GetLastOperationStatus();

						// Set filename text
						filenameText->ShowStatusText();
						if (!success)
						{
							filenameText->SetErrorStatusText(status.second);
							break;
						}
						filenameText->SetSuccessStatusText(status.second);

						// Update specified vehicle
						delete m_vehiclePrototypes[m_currentVehicle];
						m_vehiclePrototypes[m_currentVehicle] = m_vehicleBuilder.Get();
						m_vehiclePrototypes[m_currentVehicle]->SetCenter(m_mapBuilder.GetVehicleCenter());
						m_vehiclePrototypes[m_currentVehicle]->SetAngle(m_mapBuilder.GetVehicleAngle());
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
		{
			if (m_userVehicle)
			{
				m_userVehicle->Update(m_simulatedWorld->GetStaticWorld());

				if (m_userVehicle->IsActive())
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					{
						m_userVehicle->Turn(0.0);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					{
						m_userVehicle->Turn(1.0);
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					{
						m_userVehicle->DriveForward(1.0);
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					{
						m_userVehicle->DriveBackward(1.0);
					}
				}

				CoreWindow::SetViewCenter(m_userVehicle->GetCenter());
			}
			else if (m_viewTimer.Update())
			{
				sf::Vector2f m_viewCenter;
				if (m_numberOfVehicles)
				{
					auto index = m_fitnessSystem->MarkLeader(m_simulatedVehicles);
					m_viewCenter = m_simulatedVehicles[index]->GetCenter();
				}
				else
					m_viewCenter = m_userVehiclePrototype->GetCenter();

				// Update view
				auto viewCenter = CoreWindow::GetViewCenter();
				auto distance = DrawableMath::Distance(viewCenter, m_viewCenter);
				auto angle = DrawableMath::DifferenceVectorAngle(viewCenter, m_viewCenter);
				viewCenter = DrawableMath::GetEndPoint(viewCenter, angle, float(-distance * m_viewMovementOffset * CoreWindow::GetElapsedTime()));
				CoreWindow::SetViewCenter(viewCenter);
			}

			for (size_t i = 0; i < m_numberOfVehicles; ++i)
			{
				m_simulatedVehicles[i]->Update(m_simulatedWorld->GetStaticWorld());
				if (!m_simulatedVehicles[i]->IsActive())
					continue;
				const NeuronLayer& input = m_simulatedVehicles[i]->ProcessOutput();
				const NeuronLayer& output = m_artificialNeuralNetworks[i]->Update(input);
				m_simulatedVehicles[i]->ProcessInput(output);
			}

			m_simulatedWorld->Update();
			break;
		}
		case PAUSED_MODE:
		default:
			break;
	}

	for (auto& text : m_texts)
		text->Update();
}

bool StateTesting::Load()
{
	// Create texts
	m_texts[MODE_TEXT] = new StatusText({ "Mode:", "", "| [M] [P]" });
	m_texts[FILENAME_TYPE_TEXT] = new TripleText({ "Filename type:", "", "| [F]" });
	m_texts[FILENAME_TEXT] = new FilenameText<true, false>("map.bin");
	m_texts[PARAMETER_TYPE_TEXT] = new TripleText({ "Parameter type:", "", "| [P] [+] [-]" });
	m_texts[CURRENT_VEHICLE_TEXT] = new DoubleText({ "Current vehicle:" });
	m_texts[NUMBER_OF_VEHICLES_TEXT] = new DoubleText({ "Number of vehicles:" });
	m_texts[ENABLE_CHECKPOINTS_TEXT] = new DoubleText({ "Enable checkpoints:" });
	m_texts[ENABLE_USER_VEHICLE_TEXT] = new DoubleText({ "Enable user vehicle:" });
	m_texts[USER_FITNESS_TEXT] = new DoubleText({ "User fitness:" });
	m_texts[ZOOM_TEXT] = new TripleText({ "Zoom:", "", "| [+] [-]" });

	// Create observers
	m_textObservers[MODE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_modeStrings[m_mode]; });
	m_textObservers[FILENAME_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_filenameTypeStrings[m_filenameType]; });
	m_textObservers[PARAMETER_TYPE_TEXT] = new FunctionEventObserver<std::string>([&] { return m_parameterTypesStrings[m_parameterType]; });
	m_textObservers[CURRENT_VEHICLE_TEXT] = new FunctionEventObserver<std::string>([&] { return GetCurrentVehicleName(); });
	m_textObservers[NUMBER_OF_VEHICLES_TEXT] = new TypeEventObserver<size_t>(m_numberOfVehicles);
	m_textObservers[ENABLE_CHECKPOINTS_TEXT] = new FunctionEventObserver<bool>([&] { return m_enableCheckpoints; });
	m_textObservers[ENABLE_USER_VEHICLE_TEXT] = new FunctionEventObserver<bool>([&] { return m_enableUserVehicle; });
	m_textObservers[USER_FITNESS_TEXT] = new FunctionTimerObserver<std::string>([&]{ return !m_userVehicle ? "Unknown" : std::to_string(size_t(m_userVehicle->GetFitness() / m_fitnessSystem->GetMaxFitness() * 100.0)) + "%"; }, 0.5);
	m_textObservers[ZOOM_TEXT] = new FunctionEventObserver<float>([&] { return m_zoom; });

	// Set text observers
	for (size_t i = 0; i < TEXT_COUNT; ++i)
		m_texts[i]->SetObserver(m_textObservers[i]);

	// Set texts positions
	m_texts[MODE_TEXT]->SetPosition({ FontContext::Component(0), {0}, {3}, {6}, {13} });
	m_texts[FILENAME_TYPE_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {6} });
	m_texts[FILENAME_TEXT]->SetPosition({ FontContext::Component(2), {0}, {3}, {6}, {13} });
	m_texts[PARAMETER_TYPE_TEXT]->SetPosition({ FontContext::Component(5, true), {0}, {5}, {9} });
	m_texts[CURRENT_VEHICLE_TEXT]->SetPosition({ FontContext::Component(4, true), {0}, {5} });
	m_texts[NUMBER_OF_VEHICLES_TEXT]->SetPosition({ FontContext::Component(3, true), {0}, {5} });
	m_texts[ENABLE_CHECKPOINTS_TEXT]->SetPosition({ FontContext::Component(2, true), {0}, {5} });
	m_texts[ENABLE_USER_VEHICLE_TEXT]->SetPosition({ FontContext::Component(1, true), {0}, {5} });
	m_texts[USER_FITNESS_TEXT]->SetPosition({ FontContext::Component(1, true), {7}, {10} });
	m_texts[ZOOM_TEXT]->SetPosition({ FontContext::Component(1), {0}, {3}, {6} });

	CoreLogger::PrintSuccess("State \"Testing\" dependencies loaded correctly");
	return true;
}

void StateTesting::Draw()
{
	switch (m_mode)
	{
		case STOPPED_MODE:
			if (m_mapPrototype)
			{
				if (m_enableCheckpoints)
					m_mapPrototype->DrawCheckpoints();
				m_mapPrototype->DrawEdges();

				if (m_enableUserVehicle)
				{
					m_userVehiclePrototype->DrawBody();
					m_userVehiclePrototype->DrawBeams();
				}

				for (const auto& vehiclePrototype : m_vehiclePrototypes)
				{
					if (!vehiclePrototype)
						continue;
					vehiclePrototype->DrawBody();
					vehiclePrototype->DrawBeams();
				}
			}

			m_texts[FILENAME_TYPE_TEXT]->Draw();
			m_texts[FILENAME_TEXT]->Draw();
			m_texts[PARAMETER_TYPE_TEXT]->Draw();
			m_texts[CURRENT_VEHICLE_TEXT]->Draw();
			m_texts[NUMBER_OF_VEHICLES_TEXT]->Draw();
			break;
		case RUNNING_MODE:
			m_simulatedWorld->Draw();
			if (m_enableUserVehicle)
				m_texts[USER_FITNESS_TEXT]->Draw();
			m_texts[ZOOM_TEXT]->Draw();
			break;
		case PAUSED_MODE:
			m_simulatedWorld->Draw();
			if (m_enableUserVehicle)
				m_texts[USER_FITNESS_TEXT]->Draw();
			break;
		default:
			break;
	}
	
	m_texts[MODE_TEXT]->Draw();
	m_texts[ENABLE_CHECKPOINTS_TEXT]->Draw();
	m_texts[ENABLE_USER_VEHICLE_TEXT]->Draw();
}

void StateTesting::OnAddVehicle()
{
	if (m_numberOfVehicles < m_maxNumberOfVehicles)
	{
		if (m_numberOfVehicles)
		{
			VehiclePrototypes vehiclePrototypesTemp(m_numberOfVehicles + 1, nullptr);
			ArtificialNeuralNetworks artificialNeuralNetworksTemp(m_numberOfVehicles + 1, nullptr);

			for (size_t i = 0; i <= m_currentVehicle; ++i)
			{
				vehiclePrototypesTemp[i] = m_vehiclePrototypes[i];
				artificialNeuralNetworksTemp[i] = m_artificialNeuralNetworks[i];
			}

			vehiclePrototypesTemp[m_currentVehicle + 1] = nullptr;
			artificialNeuralNetworksTemp[m_currentVehicle + 1] = nullptr;

			for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
			{
				vehiclePrototypesTemp[i + 1] = m_vehiclePrototypes[i];
				artificialNeuralNetworksTemp[i + 1] = m_artificialNeuralNetworks[i];
			}

			m_vehiclePrototypes = vehiclePrototypesTemp;
			m_artificialNeuralNetworks = artificialNeuralNetworksTemp;
			++m_numberOfVehicles;
		}
		else
		{
			++m_numberOfVehicles;
			m_vehiclePrototypes.resize(m_numberOfVehicles, nullptr);
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
		delete m_vehiclePrototypes[m_currentVehicle];
		delete m_artificialNeuralNetworks[m_currentVehicle];

		for (size_t i = m_currentVehicle + 1; i < m_numberOfVehicles; ++i)
		{
			m_vehiclePrototypes[i - 1] = m_vehiclePrototypes[i];
			m_artificialNeuralNetworks[i - 1] = m_artificialNeuralNetworks[i];
		}

		m_vehiclePrototypes.pop_back();
		m_artificialNeuralNetworks.pop_back();

		if (m_currentVehicle)
			--m_currentVehicle;

		--m_numberOfVehicles;
		m_textObservers[CURRENT_VEHICLE_TEXT]->Notify();
		m_textObservers[NUMBER_OF_VEHICLES_TEXT]->Notify();
	}
}

std::string StateTesting::GetCurrentVehicleName() const
{
	if (m_numberOfVehicles == 0)
		return "None";

	std::string data = " - Body [ ] Artificial Neural Network [ ]";
	data[9] = m_vehiclePrototypes[m_currentVehicle] ? 'X' : ' ';
	data[39] = m_artificialNeuralNetworks[m_currentVehicle] ? 'X' : ' ';
	return std::string("Vehicle ") + std::to_string(m_currentVehicle) + data;
}
